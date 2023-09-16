//
// Copyright (c) Yeditepe University Computer Engineering Department, 2005.
// This file is part of HIPPY
//
// Hippy is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Hippy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hippy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "m6800.h"

/*
 * returns true if the last operation changed
 * the value stored in a memory location.
 * in such a case w will contaion the mem addr.
 */
bool CM6800::MemLocChanged(Word &w)
{
    w = mar;
    return bMemLocChanged;
}

void CM6800::Step()
{

    if (psem_reset->Lock(0))
    {
        reset();
        stack_wnd->CheckStack(0x02, (bool) runMode);
        stack_wnd->SendMessage(WM_REDRAWALL);
    }

    if (psem_nmi->Lock(0))
    {
        nmi();
        stack_wnd->CheckStack(0x00, (bool) runMode);
        stack_wnd->UpdateWindow();
    }

    if (psem_irq->Lock(0))
    {
        if (!regs.ccr.i)
        { // 07.30.2005 bug fix (martin)
            irq();
            stack_wnd->CheckStack(0x00, (bool) runMode);
            stack_wnd->UpdateWindow();
        }
    }

    opcode = memory.Read(regs.pc);

    AddressingMode addrMode = InstModes[opcode];
    int            instLength = InstDescTbl[opcode] & 0x0F;
    Word           param;

    bMemLocChanged = false;
    BYTE oldval;

    switch (instLength)
    {
    // case 0: /*INVALID*/ break;
    case 2:
        param = memory.Read(regs.pc + 1);
        break;
    case 3:
        param = memory.Read(regs.pc + 1) * 0x100;
        param |= memory.Read(regs.pc + 2);
    }

    // this is done here so that the effect of
    // branch instructions are not distorted
    regs.pc += instLength;

    switch (addrMode)
    {
    // case amNon: /*INVALID*/ break;
    case amImp:
        break;
    case amDir:
    case amExt:
        mar = param;
        mdr = memory.Read(mar);
        break;
    case amInd:
        mar = param + regs.x;
        mdr = memory.Read(mar);
        break;
    case amRel:
    {
        char c = (char) param;
        mar = regs.pc + c;
        mdr = memory.Read(mar);
    }
    break;
    case amImm:
        mdr = (BYTE) param;
        mar = param;
        break;
    }

    oldval = (BYTE) mar;

    ftype funct = functions[opcode];
    if (funct)
        (this->*funct)();
    else
        done = true;

    stack_wnd->CheckStack(opcode, (bool) runMode);

    if ((instLength > 1) && (memory.GetLastWrite() == mar) && (memory.Read(mar, true) != oldval))
    {
        bMemLocChanged = true;
    }
}

void CM6800::aba()
{
    regs.a = pAlu->Add(regs.a, regs.b);
}

void CM6800::adda()
{
    regs.a = pAlu->Add(mdr, regs.a);
}

void CM6800::addb()
{
    regs.b = pAlu->Add(mdr, regs.b);
}

void CM6800::adca()
{
    regs.a = pAlu->Addc(mdr, regs.a);
}

void CM6800::adcb()
{
    regs.b = pAlu->Addc(mdr, regs.b);
}

void CM6800::anda()
{
    regs.a = pAlu->And(mdr, regs.a);
}

void CM6800::andb()
{
    regs.b = pAlu->And(mdr, regs.b);
}

void CM6800::bita()
{
    pAlu->And(mdr, regs.a);
}

void CM6800::bitb()
{
    pAlu->And(mdr, regs.b);
}

void CM6800::clr()
{
    memory.Write(mar, 0x00);
    pAlu->Clr();
}

void CM6800::clra()
{
    regs.a = 0x00;
    pAlu->Clr();
}

void CM6800::clrb()
{
    regs.b = 0x00;
    pAlu->Clr();
}

void CM6800::cmpa()
{
    pAlu->Sub(regs.a, mdr);
}

void CM6800::cmpb()
{
    pAlu->Sub(regs.b, mdr);
}

void CM6800::cba()
{
    pAlu->Sub(regs.a, regs.b);
}

void CM6800::com()
{
    memory.Write(mar, pAlu->Com(mdr));
}

void CM6800::coma()
{
    regs.a = pAlu->Com(regs.a);
}

void CM6800::comb()
{
    regs.b = pAlu->Com(regs.b);
}

void CM6800::neg()
{
    memory.Write(mar, pAlu->Neg(mdr));
}

void CM6800::nega()
{
    regs.a = pAlu->Neg(regs.a);
}

void CM6800::negb()
{
    regs.b = pAlu->Neg(regs.b);
}

void CM6800::daa()
{
    regs.a = pAlu->Daa(regs.a);
}

void CM6800::dec()
{
    memory.Write(mar, pAlu->Dec(mdr));
}

void CM6800::deca()
{
    regs.a = pAlu->Dec(regs.a);
}

void CM6800::decb()
{
    regs.b = pAlu->Dec(regs.b);
}

void CM6800::eora()
{
    regs.a = pAlu->Eor(mdr, regs.a);
}

void CM6800::eorb()
{
    regs.b = pAlu->Eor(mdr, regs.b);
}

void CM6800::inc()
{
    memory.Write(mar, pAlu->Inc(mdr));
}

void CM6800::inca()
{
    regs.a = pAlu->Inc(regs.a);
}

void CM6800::incb()
{
    regs.b = pAlu->Inc(regs.b);
}

void CM6800::ldaa()
{
    lda_uni(regs.a);
}

void CM6800::ldab()
{
    lda_uni(regs.b);
}

void CM6800::lda_uni(BYTE &v1)
{
    regs.ccr.v = 0;
    regs.ccr.n = (mdr & 0x80) == 0x80;
    regs.ccr.z = !mdr;
    v1 = mdr;
}

void CM6800::oraa()
{
    regs.a = pAlu->Or(mdr, regs.a);
}

void CM6800::orab()
{
    regs.b = pAlu->Or(mdr, regs.b);
}

void CM6800::psha()
{
    memory.Write(regs.sp, regs.a);
    regs.sp--;
}

void CM6800::pshb()
{
    memory.Write(regs.sp, regs.b);
    regs.sp--;
}

void CM6800::pula()
{
    regs.sp++;
    regs.a = memory.Read(regs.sp);
}

void CM6800::pulb()
{
    regs.sp++;
    regs.b = memory.Read(regs.sp);
}

void CM6800::rol()
{
    memory.Write(mar, pAlu->Rol(mdr));
}

void CM6800::rola()
{
    regs.a = pAlu->Rol(regs.a);
}

void CM6800::rolb()
{
    regs.b = pAlu->Rol(regs.b);
}

void CM6800::ror()
{
    memory.Write(mar, pAlu->Ror(mdr));
}

void CM6800::rora()
{
    regs.a = pAlu->Ror(regs.a);
}

void CM6800::rorb()
{
    regs.b = pAlu->Ror(regs.b);
}

void CM6800::asl()
{
    memory.Write(mar, pAlu->Asl(mdr));
}

void CM6800::asla()
{
    regs.a = pAlu->Asl(regs.a);
}

void CM6800::aslb()
{
    regs.b = pAlu->Asl(regs.b);
}

void CM6800::asr()
{
    memory.Write(mar, pAlu->Asr(mdr));
}

void CM6800::asra()
{
    regs.a = pAlu->Asr(regs.a);
}

void CM6800::asrb()
{
    regs.b = pAlu->Asr(regs.b);
}

void CM6800::lsr()
{
    memory.Write(mar, pAlu->Lsr(mdr));
}

void CM6800::lsra()
{
    regs.a = pAlu->Lsr(regs.a);
}

void CM6800::lsrb()
{
    regs.b = pAlu->Lsr(regs.b);
}

void CM6800::staa()
{
    sta_uni(regs.a);
}

void CM6800::stab()
{
    sta_uni(regs.b);
}

void CM6800::sta_uni(BYTE v1)
{
    memory.Write(mar, v1);
    regs.ccr.v = 0;
    regs.ccr.n = ((v1 & 0x80) == 0x80);
    regs.ccr.z = !v1;
}

void CM6800::suba()
{
    regs.a = pAlu->Sub(regs.a, mdr);
}

void CM6800::subb()
{
    regs.b = pAlu->Sub(regs.b, mdr);
}

void CM6800::sba()
{
    regs.a = pAlu->Sub(regs.a, regs.b);
}

void CM6800::sbca()
{
    regs.a = pAlu->Subc(regs.a, mdr);
}

void CM6800::sbcb()
{
    regs.b = pAlu->Subc(regs.b, mdr);
}

void CM6800::tab()
{
    regs.b = regs.a;
    regs.ccr.v = 0;
    regs.ccr.n = ((regs.a & 0x80) == 0x80);
    regs.ccr.z = !regs.a;
}

void CM6800::tba()
{
    regs.a = regs.b;
    regs.ccr.v = 0;
    regs.ccr.n = ((regs.b & 0x80) == 0x80);
    regs.ccr.z = !regs.b;
}

void CM6800::tst()
{
    pAlu->Tst(mdr);
}

void CM6800::tsta()
{
    pAlu->Tst(regs.a);
}

void CM6800::tstb()
{
    pAlu->Tst(regs.b);
}

void CM6800::bra()
{
    regs.pc = mar;
}

void CM6800::bcc()
{
    if (!regs.ccr.c)
        regs.pc = mar;
}

void CM6800::bcs()
{
    if (regs.ccr.c)
        regs.pc = mar;
}

void CM6800::bne()
{
    if (!regs.ccr.z)
        regs.pc = mar;
}

void CM6800::beq()
{
    if (regs.ccr.z)
        regs.pc = mar;
}

void CM6800::bhi()
{
    if (!(regs.ccr.c || regs.ccr.z))
        regs.pc = mar;
}

void CM6800::bls()
{
    if (regs.ccr.c || regs.ccr.z)
        regs.pc = mar;
}

void CM6800::bvc()
{
    if (!regs.ccr.v)
        regs.pc = mar;
}

void CM6800::bvs()
{
    if (regs.ccr.v)
        regs.pc = mar;
}

void CM6800::bpl()
{
    if (!regs.ccr.n)
        regs.pc = mar;
}
void CM6800::bmi()
{
    if (regs.ccr.n)
        regs.pc = mar;
}

void CM6800::bge()
{
    if (!(regs.ccr.v ^ regs.ccr.n))
        regs.pc = mar;
}

void CM6800::blt()
{
    if (regs.ccr.v ^ regs.ccr.n)
        regs.pc = mar;
}

void CM6800::bgt()
{
    if ((regs.ccr.v ^ regs.ccr.n) & regs.ccr.z)
        regs.pc = mar;
}

void CM6800::ble()
{
    if (!(~(regs.ccr.v ^ regs.ccr.n) & ~regs.ccr.z))
        regs.pc = mar;
}

void CM6800::bsr()
{
    memory.Write(regs.sp, regs.pc & 0xff);
    memory.Write(regs.sp - 1, (regs.pc & 0xff00) >> 8);
    regs.sp -= 2;
    regs.pc = mar;
}

void CM6800::jmp()
{
    regs.pc = mar;
}

void CM6800::jsr()
{
    memory.Write(regs.sp, regs.pc & 0xff);
    memory.Write(regs.sp - 1, (regs.pc & 0xff00) >> 8);
    regs.sp -= 2;
    regs.pc = mar;
}

void CM6800::nop()
{
    /**/
}

void CM6800::rti()
{
    regs.sp++;
    regs.ccr.all = memory.Read(regs.sp);
    regs.sp++;
    regs.b = memory.Read(regs.sp);
    regs.sp++;
    regs.a = memory.Read(regs.sp);
    regs.sp++;
    regs.x = memory.Read(regs.sp) << 8;
    regs.sp++;
    regs.x += memory.Read(regs.sp);
    regs.sp++;
    regs.pc = memory.Read(regs.sp) << 8;
    regs.sp++;
    regs.pc += memory.Read(regs.sp);
}

void CM6800::rts()
{
    regs.sp++;
    regs.pc = (memory.Read(regs.sp) << 8);
    regs.sp++;
    regs.pc += memory.Read(regs.sp);
}

void CM6800::nmi()
{
    saveState();
    regs.ccr.i = 1; // Martin Widler 08.06.05
    regs.pc = (memory.Read(0xfffc) << 8) + memory.Read(0xfffd);
}

void CM6800::irq()
{
    saveState();
    regs.ccr.i = 1; // Martin Widler 08.06.05
    regs.pc = (memory.Read(0xfff8) << 8) + memory.Read(0xfff9);
}

void CM6800::reset()
{
    saveState();
    regs.ccr.i = 1; // reported by MARTIN WIDLER (07.30.2005)
    regs.pc = (memory.Read(0xfffe) << 8) + memory.Read(0xffff);
}

void CM6800::swi()
{
    saveState();
    regs.ccr.i = 1; // Martin Widler 08.06.05
    regs.pc = (memory.Read(0xfffa) << 8) + memory.Read(0xfffb);
}

void CM6800::wai()
{
    saveState();
    /*--------*/
    HANDLE h[3];
    h[0] = psem_irq->m_hObject;
    h[1] = psem_nmi->m_hObject;
    h[2] = psem_reset->m_hObject;
    WaitForMultipleObjects(3, h, false, INFINITE);
}

void CM6800::saveState()
{
    memory.Write(regs.sp, regs.pc & 0xff);
    memory.Write(regs.sp - 1, (regs.pc & 0xff00) >> 8);
    regs.sp -= 2;
    memory.Write(regs.sp, regs.x & 0xff);
    memory.Write(regs.sp - 1, (regs.x & 0xff00) >> 8);
    regs.sp -= 2;
    memory.Write(regs.sp, regs.a);
    regs.sp--;
    memory.Write(regs.sp, regs.b);
    regs.sp--;
    memory.Write(regs.sp, regs.ccr.all);
    regs.sp--;
}

////////////////////
// IND REG & STK PTR

void CM6800::cpx()
{
    Word w;

    if (InstModes[opcode] != amImm)
        w = (memory.Read(mar) << 8) + memory.Read(mar + 1);
    else
        w = mar;

    pAlu->SubExt(regs.x, w);
}

void CM6800::dex()
{
    regs.x--;
    regs.ccr.z = !regs.x;
}

void CM6800::des()
{
    regs.sp--;
}

void CM6800::inx()
{
    regs.x++;
    regs.ccr.z = !regs.x;
}

void CM6800::ins()
{
    regs.sp++;
}

void CM6800::ldx()
{
    if (InstModes[opcode] != amImm)
        regs.x = (memory.Read(mar) << 8) + memory.Read(mar + 1);
    else
        regs.x = mar;
    regs.ccr.v = 0;
    regs.ccr.z = !regs.x;
    regs.ccr.n = ((regs.x & 0x8000) == 0x8000);
}

void CM6800::lds()
{
    if (InstModes[opcode] != amImm)
        regs.sp = (memory.Read(mar) << 8) + memory.Read(mar + 1);
    else
        regs.sp = mar;
    regs.ccr.v = 0;
    regs.ccr.z = !regs.sp;
    regs.ccr.n = ((regs.sp & 0x8000) == 0x8000);
}

void CM6800::stx()
{
    memory.Write(mar, (regs.x & 0xff00) >> 8);
    memory.Write(mar + 1, (regs.x & 0x00ff));
    regs.ccr.v = 0;
    regs.ccr.z = !regs.x;
    regs.ccr.n = ((regs.x & 0x8000) == 0x8000);
}

void CM6800::sts()
{
    memory.Write(mar, (regs.sp & 0xff00) >> 8);
    memory.Write(mar + 1, (regs.sp & 0x00ff));
    regs.ccr.v = 0;
    regs.ccr.z = !regs.sp;
    regs.ccr.n = ((regs.sp & 0x8000) == 0x8000);
}

void CM6800::txs()
{
    regs.sp = regs.x - 1;
}

void CM6800::tsx()
{
    regs.x = regs.sp + 1;
}

/////////////////////
// CCR OPER.

void CM6800::clc()
{
    regs.ccr.c = 0;
}

void CM6800::sec()
{
    regs.ccr.c = 1;
}

void CM6800::clv()
{
    regs.ccr.v = 0;
}

void CM6800::sev()
{
    regs.ccr.v = 1;
}

void CM6800::cli()
{
    regs.ccr.i = 0;
}

void CM6800::sei()
{
    regs.ccr.i = 1;
}

void CM6800::tap()
{
    regs.ccr.all = regs.a;
    regs.ccr.rsv = 0x03;
}

void CM6800::tpa()
{
    regs.a = regs.ccr.all;
}

// constructor
CM6800::CM6800(CStackWnd *psw)
{
    initFunctions();
    stack_wnd = psw;
    regs.ccr.rsv = 0x03; // most significant bits are 11
    regs.ccr.i = 1;
    pAlu = new ALU(&regs.ccr);
    psem_nmi = new CSemaphore(0, 1000, INT_NMI);
    psem_irq = new CSemaphore(0, 1000, INT_IRQ);
    psem_reset = new CSemaphore(0, 1000, INT_RST);
}

// destructor
CM6800::~CM6800()
{
    delete psem_irq;
    delete psem_nmi;
    delete psem_reset;
}

void CM6800::initFunctions()
{
    Word op = 0;
    for (op = 0; op <= 0xff; op++)
        switch (op)
        {
            // MEMORY & ACCUMULATOR OPERATIONS
        case 0x8B:
        case 0x9B:
        case 0xAB:
        case 0xBB:
            functions[op] = &CM6800::adda;
            break;
        case 0xCB:
        case 0xDB:
        case 0xEB:
        case 0xFB:
            functions[op] = &CM6800::addb;
            break;
        case 0x1B:
            functions[op] = &CM6800::aba;
            break;
        case 0x89:
        case 0x99:
        case 0xA9:
        case 0xB9:
            functions[op] = &CM6800::adca;
            break;
        case 0xc9:
        case 0xd9:
        case 0xe9:
        case 0xf9:
            functions[op] = &CM6800::adcb;
            break;
        case 0x84:
        case 0x94:
        case 0xa4:
        case 0xb4:
            functions[op] = &CM6800::anda;
            break;
        case 0xc4:
        case 0xd4:
        case 0xe4:
        case 0xf4:
            functions[op] = &CM6800::andb;
            break;
        case 0x85:
        case 0x95:
        case 0xa5:
        case 0xb5:
            functions[op] = &CM6800::bita;
            break;
        case 0xc5:
        case 0xd5:
        case 0xe5:
        case 0xf5:
            functions[op] = &CM6800::bitb;
            break;
        case 0x4f:
            functions[op] = &CM6800::clra;
            break;
        case 0x5f:
            functions[op] = &CM6800::clrb;
            break;
        case 0x6f:
        case 0x7f:
            functions[op] = &CM6800::clr;
            break;
        case 0x81:
        case 0x91:
        case 0xa1:
        case 0xb1:
            functions[op] = &CM6800::cmpa;
            break;
        case 0xc1:
        case 0xd1:
        case 0xe1:
        case 0xf1:
            functions[op] = &CM6800::cmpb;
            break;
        case 0x11:
            functions[op] = &CM6800::cba;
            break;
        case 0x43:
            functions[op] = &CM6800::coma;
            break;
        case 0x53:
            functions[op] = &CM6800::comb;
            break;
        case 0x63:
        case 0x73:
            functions[op] = &CM6800::com;
            break;
        case 0x40:
            functions[op] = &CM6800::nega;
            break;
        case 0x50:
            functions[op] = &CM6800::negb;
            break;
        case 0x60:
        case 0x70:
            functions[op] = &CM6800::neg;
            break;
        case 0x19:
            functions[op] = &CM6800::daa;
            break;
        case 0x6a:
        case 0x7a:
            functions[op] = &CM6800::dec;
            break;
        case 0x4a:
            functions[op] = &CM6800::deca;
            break;
        case 0x5a:
            functions[op] = &CM6800::decb;
            break;
        case 0x88:
        case 0x98:
        case 0xa8:
        case 0xb8:
            functions[op] = &CM6800::eora;
            break;
        case 0xc8:
        case 0xd8:
        case 0xe8:
        case 0xf8:
            functions[op] = &CM6800::eorb;
            break;
        case 0x6c:
        case 0x7c:
            functions[op] = &CM6800::inc;
            break;
        case 0x4c:
            functions[op] = &CM6800::inca;
            break;
        case 0x5c:
            functions[op] = &CM6800::incb;
            break;
        case 0x86:
        case 0x96:
        case 0xa6:
        case 0xb6:
            functions[op] = &CM6800::ldaa;
            break;
        case 0xc6:
        case 0xd6:
        case 0xe6:
        case 0xf6:
            functions[op] = &CM6800::ldab;
            break;
        case 0x8a:
        case 0x9a:
        case 0xaa:
        case 0xba:
            functions[op] = &CM6800::oraa;
            break;
        case 0xca:
        case 0xda:
        case 0xea:
        case 0xfa:
            functions[op] = &CM6800::orab;
            break;
        case 0x36:
            functions[op] = &CM6800::psha;
            break;
        case 0x37:
            functions[op] = &CM6800::pshb;
            break;
        case 0x32:
            functions[op] = &CM6800::pula;
            break;
        case 0x33:
            functions[op] = &CM6800::pulb;
            break;
        case 0x49:
            functions[op] = &CM6800::rola;
            break;
        case 0x59:
            functions[op] = &CM6800::rolb;
            break;
        case 0x69:
        case 0x79:
            functions[op] = &CM6800::rol;
            break;
        case 0x46:
            functions[op] = &CM6800::rora;
            break;
        case 0x56:
            functions[op] = &CM6800::rorb;
            break;
        case 0x66:
        case 0x76:
            functions[op] = &CM6800::ror;
            break;
        case 0x48:
            functions[op] = &CM6800::asla;
            break;
        case 0x58:
            functions[op] = &CM6800::aslb;
            break;
        case 0x68:
        case 0x78:
            functions[op] = &CM6800::asl;
            break;
        case 0x47:
            functions[op] = &CM6800::asra;
            break;
        case 0x57:
            functions[op] = &CM6800::asrb;
            break;
        case 0x67:
        case 0x77:
            functions[op] = &CM6800::asr;
            break;
        case 0x44:
            functions[op] = &CM6800::lsra;
            break;
        case 0x54:
            functions[op] = &CM6800::lsrb;
            break;
        case 0x64:
        case 0x74:
            functions[op] = &CM6800::lsr;
            break;
        case 0x97:
        case 0xa7:
        case 0xb7:
            functions[op] = &CM6800::staa;
            break;
        case 0xd7:
        case 0xe7:
        case 0xf7:
            functions[op] = &CM6800::stab;
            break;
        case 0x80:
        case 0x90:
        case 0xa0:
        case 0xb0:
            functions[op] = &CM6800::suba;
            break;
        case 0xc0:
        case 0xd0:
        case 0xe0:
        case 0xf0:
            functions[op] = &CM6800::subb;
            break;
        case 0x10:
            functions[op] = &CM6800::sba;
            break;
        case 0x82:
        case 0x92:
        case 0xa2:
        case 0xb2:
            functions[op] = &CM6800::sbca;
            break;
        case 0xc2:
        case 0xd2:
        case 0xe2:
        case 0xf2:
            functions[op] = &CM6800::sbcb;
            break;
        case 0x16:
            functions[op] = &CM6800::tab;
            break;
        case 0x17:
            functions[op] = &CM6800::tba;
            break;
        case 0x4d:
            functions[op] = &CM6800::tsta;
            break;
        case 0x5d:
            functions[op] = &CM6800::tstb;
            break;
        case 0x6d:
        case 0x7d:
            functions[op] = &CM6800::tst;
            break;
            // BRANCH OPERATIONS
        case 0x20:
            functions[op] = &CM6800::bra;
            break;
        case 0x22:
            functions[op] = &CM6800::bhi;
            break;
        case 0x23:
            functions[op] = &CM6800::bls;
            break;
        case 0x24:
            functions[op] = &CM6800::bcc;
            break;
        case 0x25:
            functions[op] = &CM6800::bcs;
            break;
        case 0x26:
            functions[op] = &CM6800::bne;
            break;
        case 0x27:
            functions[op] = &CM6800::beq;
            break;
        case 0x28:
            functions[op] = &CM6800::bvc;
            break;
        case 0x29:
            functions[op] = &CM6800::bvs;
            break;
        case 0x2a:
            functions[op] = &CM6800::bpl;
            break;
        case 0x2b:
            functions[op] = &CM6800::bmi;
            break;
        case 0x2c:
            functions[op] = &CM6800::bge;
            break;
        case 0x2d:
            functions[op] = &CM6800::blt;
            break;
        case 0x2e:
            functions[op] = &CM6800::bgt;
            break;
        case 0x2f:
            functions[op] = &CM6800::ble;
            break;
        case 0x8d:
            functions[op] = &CM6800::bsr;
            break;
        case 0xad:
        case 0xbd:
            functions[op] = &CM6800::jsr;
            break;
        case 0x6e:
        case 0x7e:
            functions[op] = &CM6800::jmp;
            break;
        case 0x01:
            functions[op] = &CM6800::nop;
            break;
        case 0x39:
            functions[op] = &CM6800::rts;
            break;
        case 0x3b:
            functions[op] = &CM6800::rti;
            break;
        case 0x3e:
            functions[op] = &CM6800::wai;
            break;
        case 0x3f:
            functions[op] = &CM6800::swi;
            break;
            // INDEX & STACK
        case 0x8c:
        case 0x9c:
        case 0xac:
        case 0xbc:
            functions[op] = &CM6800::cpx;
            break;
        case 0x09:
            functions[op] = &CM6800::dex;
            break;
        case 0x34:
            functions[op] = &CM6800::des;
            break;
        case 0x08:
            functions[op] = &CM6800::inx;
            break;
        case 0x31:
            functions[op] = &CM6800::ins;
            break;
        case 0xce:
        case 0xde:
        case 0xee:
        case 0xfe:
            functions[op] = &CM6800::ldx;
            break;
        case 0x8e:
        case 0x9e:
        case 0xae:
        case 0xbe:
            functions[op] = &CM6800::lds;
            break;
        case 0xdf:
        case 0xef:
        case 0xff:
            functions[op] = &CM6800::stx;
            break;
        case 0x9f:
        case 0xaf:
        case 0xbf:
            functions[op] = &CM6800::sts;
            break;
        case 0x35:
            functions[op] = &CM6800::txs;
            break;
        case 0x30:
            functions[op] = &CM6800::tsx;
            break;
            // CCR OPS
        case 0x0c:
            functions[op] = &CM6800::clc;
            break;
        case 0x0d:
            functions[op] = &CM6800::sec;
            break;
        case 0x0a:
            functions[op] = &CM6800::clv;
            break;
        case 0x0b:
            functions[op] = &CM6800::sev;
            break;
        case 0x0e:
            functions[op] = &CM6800::cli;
            break;
        case 0x0f:
            functions[op] = &CM6800::sei;
            break;
        case 0x06:
            functions[op] = &CM6800::tap;
            break;
        case 0x07:
            functions[op] = &CM6800::tpa;
            break;
        default:
            functions[op] = NULL;
        }
}