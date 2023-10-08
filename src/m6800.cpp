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

#include "disassembler.h"

#include <afxmt.h>

/*
 * returns true if the last operation changed
 * the value stored in a memory location.
 * in such a case w will contaion the mem addr.
 */
bool CM6800::MemLocChanged(Word &w)
{
    w = m_mar;
    return m_memLocChanged;
}

void CM6800::Step()
{
    if (m_reset->Lock(0))
    {
        reset();
        m_stackWnd->CheckStack(0x02, (bool) m_runMode);
        m_stackWnd->SendMessage(WM_REDRAWALL);
    }

    if (m_nmi->Lock(0))
    {
        nmi();
        m_stackWnd->CheckStack(0x00, (bool) m_runMode);
        m_stackWnd->UpdateWindow();
    }

    if (m_irq->Lock(0))
    {
        if (!m_regs.ccr.i)
        { // 07.30.2005 bug fix (martin)
            irq();
            m_stackWnd->CheckStack(0x00, (bool) m_runMode);
            m_stackWnd->UpdateWindow();
        }
    }

    m_opcode = m_memory.Read(m_regs.pc);

    AddressingMode addrMode = g_instModes[m_opcode];
    int            instLength = g_instDescTbl[m_opcode] & 0x0F;
    Word           param;

    m_memLocChanged = false;
    BYTE oldval;

    switch (instLength)
    {
    // case 0: /*INVALID*/ break;
    case 2:
        param = m_memory.Read(m_regs.pc + 1);
        break;
    case 3:
        param = m_memory.Read(m_regs.pc + 1) * 0x100;
        param |= m_memory.Read(m_regs.pc + 2);
    }

    // this is done here so that the effect of
    // branch instructions are not distorted
    m_regs.pc += instLength;

    switch (addrMode)
    {
    // case amNon: /*INVALID*/ break;
    case amImp:
        break;
    case amDir:
    case amExt:
        m_mar = param;
        m_mdr = m_memory.Read(m_mar);
        break;
    case amInd:
        m_mar = param + m_regs.x;
        m_mdr = m_memory.Read(m_mar);
        break;
    case amRel:
    {
        char c = (char) param;
        m_mar = m_regs.pc + c;
        m_mdr = m_memory.Read(m_mar);
    }
    break;
    case amImm:
        m_mdr = (BYTE) param;
        m_mar = param;
        break;
    }

    oldval = (BYTE) m_mar;

    if (FnType funct = m_functions[m_opcode])
    {
        (this->*funct)();
    }
    else
    {
        m_done = true;
    }

    m_stackWnd->CheckStack(m_opcode, (bool) m_runMode);

    if ((instLength > 1) && (m_memory.GetLastWrite() == m_mar) && (m_memory.Read(m_mar, true) != oldval))
    {
        m_memLocChanged = true;
    }
}

void CM6800::aba()
{
    m_regs.a = m_alu->Add(m_regs.a, m_regs.b);
}

void CM6800::adda()
{
    m_regs.a = m_alu->Add(m_mdr, m_regs.a);
}

void CM6800::addb()
{
    m_regs.b = m_alu->Add(m_mdr, m_regs.b);
}

void CM6800::adca()
{
    m_regs.a = m_alu->Addc(m_mdr, m_regs.a);
}

void CM6800::adcb()
{
    m_regs.b = m_alu->Addc(m_mdr, m_regs.b);
}

void CM6800::anda()
{
    m_regs.a = m_alu->And(m_mdr, m_regs.a);
}

void CM6800::andb()
{
    m_regs.b = m_alu->And(m_mdr, m_regs.b);
}

void CM6800::bita()
{
    m_alu->And(m_mdr, m_regs.a);
}

void CM6800::bitb()
{
    m_alu->And(m_mdr, m_regs.b);
}

void CM6800::clr()
{
    m_memory.Write(m_mar, 0x00);
    m_alu->Clr();
}

void CM6800::clra()
{
    m_regs.a = 0x00;
    m_alu->Clr();
}

void CM6800::clrb()
{
    m_regs.b = 0x00;
    m_alu->Clr();
}

void CM6800::cmpa()
{
    m_alu->Sub(m_regs.a, m_mdr);
}

void CM6800::cmpb()
{
    m_alu->Sub(m_regs.b, m_mdr);
}

void CM6800::cba()
{
    m_alu->Sub(m_regs.a, m_regs.b);
}

void CM6800::com()
{
    m_memory.Write(m_mar, m_alu->Com(m_mdr));
}

void CM6800::coma()
{
    m_regs.a = m_alu->Com(m_regs.a);
}

void CM6800::comb()
{
    m_regs.b = m_alu->Com(m_regs.b);
}

void CM6800::neg()
{
    m_memory.Write(m_mar, m_alu->Neg(m_mdr));
}

void CM6800::nega()
{
    m_regs.a = m_alu->Neg(m_regs.a);
}

void CM6800::negb()
{
    m_regs.b = m_alu->Neg(m_regs.b);
}

void CM6800::daa()
{
    m_regs.a = m_alu->Daa(m_regs.a);
}

void CM6800::dec()
{
    m_memory.Write(m_mar, m_alu->Dec(m_mdr));
}

void CM6800::deca()
{
    m_regs.a = m_alu->Dec(m_regs.a);
}

void CM6800::decb()
{
    m_regs.b = m_alu->Dec(m_regs.b);
}

void CM6800::eora()
{
    m_regs.a = m_alu->Eor(m_mdr, m_regs.a);
}

void CM6800::eorb()
{
    m_regs.b = m_alu->Eor(m_mdr, m_regs.b);
}

void CM6800::inc()
{
    m_memory.Write(m_mar, m_alu->Inc(m_mdr));
}

void CM6800::inca()
{
    m_regs.a = m_alu->Inc(m_regs.a);
}

void CM6800::incb()
{
    m_regs.b = m_alu->Inc(m_regs.b);
}

void CM6800::ldaa()
{
    lda_uni(m_regs.a);
}

void CM6800::ldab()
{
    lda_uni(m_regs.b);
}

void CM6800::lda_uni(BYTE &v1)
{
    m_regs.ccr.v = 0;
    m_regs.ccr.n = (m_mdr & 0x80) == 0x80;
    m_regs.ccr.z = !m_mdr;
    v1 = m_mdr;
}

void CM6800::oraa()
{
    m_regs.a = m_alu->Or(m_mdr, m_regs.a);
}

void CM6800::orab()
{
    m_regs.b = m_alu->Or(m_mdr, m_regs.b);
}

void CM6800::psha()
{
    m_memory.Write(m_regs.sp, m_regs.a);
    m_regs.sp--;
}

void CM6800::pshb()
{
    m_memory.Write(m_regs.sp, m_regs.b);
    m_regs.sp--;
}

void CM6800::pula()
{
    m_regs.sp++;
    m_regs.a = m_memory.Read(m_regs.sp);
}

void CM6800::pulb()
{
    m_regs.sp++;
    m_regs.b = m_memory.Read(m_regs.sp);
}

void CM6800::rol()
{
    m_memory.Write(m_mar, m_alu->Rol(m_mdr));
}

void CM6800::rola()
{
    m_regs.a = m_alu->Rol(m_regs.a);
}

void CM6800::rolb()
{
    m_regs.b = m_alu->Rol(m_regs.b);
}

void CM6800::ror()
{
    m_memory.Write(m_mar, m_alu->Ror(m_mdr));
}

void CM6800::rora()
{
    m_regs.a = m_alu->Ror(m_regs.a);
}

void CM6800::rorb()
{
    m_regs.b = m_alu->Ror(m_regs.b);
}

void CM6800::asl()
{
    m_memory.Write(m_mar, m_alu->Asl(m_mdr));
}

void CM6800::asla()
{
    m_regs.a = m_alu->Asl(m_regs.a);
}

void CM6800::aslb()
{
    m_regs.b = m_alu->Asl(m_regs.b);
}

void CM6800::asr()
{
    m_memory.Write(m_mar, m_alu->Asr(m_mdr));
}

void CM6800::asra()
{
    m_regs.a = m_alu->Asr(m_regs.a);
}

void CM6800::asrb()
{
    m_regs.b = m_alu->Asr(m_regs.b);
}

void CM6800::lsr()
{
    m_memory.Write(m_mar, m_alu->Lsr(m_mdr));
}

void CM6800::lsra()
{
    m_regs.a = m_alu->Lsr(m_regs.a);
}

void CM6800::lsrb()
{
    m_regs.b = m_alu->Lsr(m_regs.b);
}

void CM6800::staa()
{
    sta_uni(m_regs.a);
}

void CM6800::stab()
{
    sta_uni(m_regs.b);
}

void CM6800::sta_uni(BYTE v1)
{
    m_memory.Write(m_mar, v1);
    m_regs.ccr.v = 0;
    m_regs.ccr.n = ((v1 & 0x80) == 0x80);
    m_regs.ccr.z = !v1;
}

void CM6800::suba()
{
    m_regs.a = m_alu->Sub(m_regs.a, m_mdr);
}

void CM6800::subb()
{
    m_regs.b = m_alu->Sub(m_regs.b, m_mdr);
}

void CM6800::sba()
{
    m_regs.a = m_alu->Sub(m_regs.a, m_regs.b);
}

void CM6800::sbca()
{
    m_regs.a = m_alu->Subc(m_regs.a, m_mdr);
}

void CM6800::sbcb()
{
    m_regs.b = m_alu->Subc(m_regs.b, m_mdr);
}

void CM6800::tab()
{
    m_regs.b = m_regs.a;
    m_regs.ccr.v = 0;
    m_regs.ccr.n = ((m_regs.a & 0x80) == 0x80);
    m_regs.ccr.z = !m_regs.a;
}

void CM6800::tba()
{
    m_regs.a = m_regs.b;
    m_regs.ccr.v = 0;
    m_regs.ccr.n = ((m_regs.b & 0x80) == 0x80);
    m_regs.ccr.z = !m_regs.b;
}

void CM6800::tst()
{
    m_alu->Tst(m_mdr);
}

void CM6800::tsta()
{
    m_alu->Tst(m_regs.a);
}

void CM6800::tstb()
{
    m_alu->Tst(m_regs.b);
}

void CM6800::bra()
{
    m_regs.pc = m_mar;
}

void CM6800::bcc()
{
    if (!m_regs.ccr.c)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bcs()
{
    if (m_regs.ccr.c)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bne()
{
    if (!m_regs.ccr.z)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::beq()
{
    if (m_regs.ccr.z)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bhi()
{
    if (!(m_regs.ccr.c || m_regs.ccr.z))
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bls()
{
    if (m_regs.ccr.c || m_regs.ccr.z)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bvc()
{
    if (!m_regs.ccr.v)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bvs()
{
    if (m_regs.ccr.v)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bpl()
{
    if (!m_regs.ccr.n)
    {
        m_regs.pc = m_mar;
    }
}
void CM6800::bmi()
{
    if (m_regs.ccr.n)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bge()
{
    if (!(m_regs.ccr.v ^ m_regs.ccr.n))
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::blt()
{
    if (m_regs.ccr.v ^ m_regs.ccr.n)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bgt()
{
    if ((m_regs.ccr.v ^ m_regs.ccr.n) & m_regs.ccr.z)
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::ble()
{
    if (!(~(m_regs.ccr.v ^ m_regs.ccr.n) & ~m_regs.ccr.z))
    {
        m_regs.pc = m_mar;
    }
}

void CM6800::bsr()
{
    m_memory.Write(m_regs.sp, m_regs.pc & 0xff);
    m_memory.Write(m_regs.sp - 1, (m_regs.pc & 0xff00) >> 8);
    m_regs.sp -= 2;
    m_regs.pc = m_mar;
}

void CM6800::jmp()
{
    m_regs.pc = m_mar;
}

void CM6800::jsr()
{
    m_memory.Write(m_regs.sp, m_regs.pc & 0xff);
    m_memory.Write(m_regs.sp - 1, (m_regs.pc & 0xff00) >> 8);
    m_regs.sp -= 2;
    m_regs.pc = m_mar;
}

void CM6800::nop()
{
}

void CM6800::rti()
{
    m_regs.sp++;
    m_regs.ccr.all = m_memory.Read(m_regs.sp);
    m_regs.sp++;
    m_regs.b = m_memory.Read(m_regs.sp);
    m_regs.sp++;
    m_regs.a = m_memory.Read(m_regs.sp);
    m_regs.sp++;
    m_regs.x = m_memory.Read(m_regs.sp) << 8;
    m_regs.sp++;
    m_regs.x += m_memory.Read(m_regs.sp);
    m_regs.sp++;
    m_regs.pc = m_memory.Read(m_regs.sp) << 8;
    m_regs.sp++;
    m_regs.pc += m_memory.Read(m_regs.sp);
}

void CM6800::rts()
{
    m_regs.sp++;
    m_regs.pc = (m_memory.Read(m_regs.sp) << 8);
    m_regs.sp++;
    m_regs.pc += m_memory.Read(m_regs.sp);
}

void CM6800::nmi()
{
    saveState();
    m_regs.ccr.i = 1; // Martin Widler 08.06.05
    m_regs.pc = (m_memory.Read(0xfffc) << 8) + m_memory.Read(0xfffd);
}

void CM6800::irq()
{
    saveState();
    m_regs.ccr.i = 1; // Martin Widler 08.06.05
    m_regs.pc = (m_memory.Read(0xfff8) << 8) + m_memory.Read(0xfff9);
}

void CM6800::reset()
{
    saveState();
    m_regs.ccr.i = 1; // reported by MARTIN WIDLER (07.30.2005)
    m_regs.pc = (m_memory.Read(0xfffe) << 8) + m_memory.Read(0xffff);
}

void CM6800::swi()
{
    saveState();
    m_regs.ccr.i = 1; // Martin Widler 08.06.05
    m_regs.pc = (m_memory.Read(0xfffa) << 8) + m_memory.Read(0xfffb);
}

void CM6800::wai()
{
    saveState();
    /*--------*/
    HANDLE h[3];
    h[0] = m_irq->m_hObject;
    h[1] = m_nmi->m_hObject;
    h[2] = m_reset->m_hObject;
    WaitForMultipleObjects(3, h, false, INFINITE);
}

void CM6800::saveState()
{
    m_memory.Write(m_regs.sp, m_regs.pc & 0xff);
    m_memory.Write(m_regs.sp - 1, (m_regs.pc & 0xff00) >> 8);
    m_regs.sp -= 2;
    m_memory.Write(m_regs.sp, m_regs.x & 0xff);
    m_memory.Write(m_regs.sp - 1, (m_regs.x & 0xff00) >> 8);
    m_regs.sp -= 2;
    m_memory.Write(m_regs.sp, m_regs.a);
    m_regs.sp--;
    m_memory.Write(m_regs.sp, m_regs.b);
    m_regs.sp--;
    m_memory.Write(m_regs.sp, m_regs.ccr.all);
    m_regs.sp--;
}

////////////////////
// IND REG & STK PTR

void CM6800::cpx()
{
    Word w;

    if (g_instModes[m_opcode] != amImm)
    {
        w = (m_memory.Read(m_mar) << 8) + m_memory.Read(m_mar + 1);
    }
    else
    {
        w = m_mar;
    }

    m_alu->SubExt(m_regs.x, w);
}

void CM6800::dex()
{
    m_regs.x--;
    m_regs.ccr.z = !m_regs.x;
}

void CM6800::des()
{
    m_regs.sp--;
}

void CM6800::inx()
{
    m_regs.x++;
    m_regs.ccr.z = !m_regs.x;
}

void CM6800::ins()
{
    m_regs.sp++;
}

void CM6800::ldx()
{
    if (g_instModes[m_opcode] != amImm)
    {
        m_regs.x = (m_memory.Read(m_mar) << 8) + m_memory.Read(m_mar + 1);
    }
    else
    {
        m_regs.x = m_mar;
    }
    m_regs.ccr.v = 0;
    m_regs.ccr.z = !m_regs.x;
    m_regs.ccr.n = ((m_regs.x & 0x8000) == 0x8000);
}

void CM6800::lds()
{
    if (g_instModes[m_opcode] != amImm)
    {
        m_regs.sp = (m_memory.Read(m_mar) << 8) + m_memory.Read(m_mar + 1);
    }
    else
    {
        m_regs.sp = m_mar;
    }
    m_regs.ccr.v = 0;
    m_regs.ccr.z = !m_regs.sp;
    m_regs.ccr.n = ((m_regs.sp & 0x8000) == 0x8000);
}

void CM6800::stx()
{
    m_memory.Write(m_mar, (m_regs.x & 0xff00) >> 8);
    m_memory.Write(m_mar + 1, (m_regs.x & 0x00ff));
    m_regs.ccr.v = 0;
    m_regs.ccr.z = !m_regs.x;
    m_regs.ccr.n = ((m_regs.x & 0x8000) == 0x8000);
}

void CM6800::sts()
{
    m_memory.Write(m_mar, (m_regs.sp & 0xff00) >> 8);
    m_memory.Write(m_mar + 1, (m_regs.sp & 0x00ff));
    m_regs.ccr.v = 0;
    m_regs.ccr.z = !m_regs.sp;
    m_regs.ccr.n = ((m_regs.sp & 0x8000) == 0x8000);
}

void CM6800::txs()
{
    m_regs.sp = m_regs.x - 1;
}

void CM6800::tsx()
{
    m_regs.x = m_regs.sp + 1;
}

/////////////////////
// CCR OPER.

void CM6800::clc()
{
    m_regs.ccr.c = 0;
}

void CM6800::sec()
{
    m_regs.ccr.c = 1;
}

void CM6800::clv()
{
    m_regs.ccr.v = 0;
}

void CM6800::sev()
{
    m_regs.ccr.v = 1;
}

void CM6800::cli()
{
    m_regs.ccr.i = 0;
}

void CM6800::sei()
{
    m_regs.ccr.i = 1;
}

void CM6800::tap()
{
    m_regs.ccr.all = m_regs.a;
    m_regs.ccr.rsv = 0x03;
}

void CM6800::tpa()
{
    m_regs.a = m_regs.ccr.all;
}

// constructor
CM6800::CM6800(CStackWnd *psw)
{
    initFunctions();
    m_stackWnd = psw;
    m_regs.ccr.rsv = 0x03; // most significant bits are 11
    m_regs.ccr.i = 1;
    m_alu = new ALU(&m_regs.ccr);
    m_nmi = new CSemaphore(0, 1000, INT_NMI);
    m_irq = new CSemaphore(0, 1000, INT_IRQ);
    m_reset = new CSemaphore(0, 1000, INT_RST);
}

// destructor
CM6800::~CM6800()
{
    delete m_reset;
    delete m_nmi;
    delete m_irq;
    delete m_alu;
}

void CM6800::initFunctions()
{
    for (Word op = 0; op <= 0xff; op++)
    {
        switch (op)
        {
        // MEMORY & ACCUMULATOR OPERATIONS
        case 0x8B:
        case 0x9B:
        case 0xAB:
        case 0xBB:
            m_functions[op] = &CM6800::adda;
            break;
        case 0xCB:
        case 0xDB:
        case 0xEB:
        case 0xFB:
            m_functions[op] = &CM6800::addb;
            break;
        case 0x1B:
            m_functions[op] = &CM6800::aba;
            break;
        case 0x89:
        case 0x99:
        case 0xA9:
        case 0xB9:
            m_functions[op] = &CM6800::adca;
            break;
        case 0xc9:
        case 0xd9:
        case 0xe9:
        case 0xf9:
            m_functions[op] = &CM6800::adcb;
            break;
        case 0x84:
        case 0x94:
        case 0xa4:
        case 0xb4:
            m_functions[op] = &CM6800::anda;
            break;
        case 0xc4:
        case 0xd4:
        case 0xe4:
        case 0xf4:
            m_functions[op] = &CM6800::andb;
            break;
        case 0x85:
        case 0x95:
        case 0xa5:
        case 0xb5:
            m_functions[op] = &CM6800::bita;
            break;
        case 0xc5:
        case 0xd5:
        case 0xe5:
        case 0xf5:
            m_functions[op] = &CM6800::bitb;
            break;
        case 0x4f:
            m_functions[op] = &CM6800::clra;
            break;
        case 0x5f:
            m_functions[op] = &CM6800::clrb;
            break;
        case 0x6f:
        case 0x7f:
            m_functions[op] = &CM6800::clr;
            break;
        case 0x81:
        case 0x91:
        case 0xa1:
        case 0xb1:
            m_functions[op] = &CM6800::cmpa;
            break;
        case 0xc1:
        case 0xd1:
        case 0xe1:
        case 0xf1:
            m_functions[op] = &CM6800::cmpb;
            break;
        case 0x11:
            m_functions[op] = &CM6800::cba;
            break;
        case 0x43:
            m_functions[op] = &CM6800::coma;
            break;
        case 0x53:
            m_functions[op] = &CM6800::comb;
            break;
        case 0x63:
        case 0x73:
            m_functions[op] = &CM6800::com;
            break;
        case 0x40:
            m_functions[op] = &CM6800::nega;
            break;
        case 0x50:
            m_functions[op] = &CM6800::negb;
            break;
        case 0x60:
        case 0x70:
            m_functions[op] = &CM6800::neg;
            break;
        case 0x19:
            m_functions[op] = &CM6800::daa;
            break;
        case 0x6a:
        case 0x7a:
            m_functions[op] = &CM6800::dec;
            break;
        case 0x4a:
            m_functions[op] = &CM6800::deca;
            break;
        case 0x5a:
            m_functions[op] = &CM6800::decb;
            break;
        case 0x88:
        case 0x98:
        case 0xa8:
        case 0xb8:
            m_functions[op] = &CM6800::eora;
            break;
        case 0xc8:
        case 0xd8:
        case 0xe8:
        case 0xf8:
            m_functions[op] = &CM6800::eorb;
            break;
        case 0x6c:
        case 0x7c:
            m_functions[op] = &CM6800::inc;
            break;
        case 0x4c:
            m_functions[op] = &CM6800::inca;
            break;
        case 0x5c:
            m_functions[op] = &CM6800::incb;
            break;
        case 0x86:
        case 0x96:
        case 0xa6:
        case 0xb6:
            m_functions[op] = &CM6800::ldaa;
            break;
        case 0xc6:
        case 0xd6:
        case 0xe6:
        case 0xf6:
            m_functions[op] = &CM6800::ldab;
            break;
        case 0x8a:
        case 0x9a:
        case 0xaa:
        case 0xba:
            m_functions[op] = &CM6800::oraa;
            break;
        case 0xca:
        case 0xda:
        case 0xea:
        case 0xfa:
            m_functions[op] = &CM6800::orab;
            break;
        case 0x36:
            m_functions[op] = &CM6800::psha;
            break;
        case 0x37:
            m_functions[op] = &CM6800::pshb;
            break;
        case 0x32:
            m_functions[op] = &CM6800::pula;
            break;
        case 0x33:
            m_functions[op] = &CM6800::pulb;
            break;
        case 0x49:
            m_functions[op] = &CM6800::rola;
            break;
        case 0x59:
            m_functions[op] = &CM6800::rolb;
            break;
        case 0x69:
        case 0x79:
            m_functions[op] = &CM6800::rol;
            break;
        case 0x46:
            m_functions[op] = &CM6800::rora;
            break;
        case 0x56:
            m_functions[op] = &CM6800::rorb;
            break;
        case 0x66:
        case 0x76:
            m_functions[op] = &CM6800::ror;
            break;
        case 0x48:
            m_functions[op] = &CM6800::asla;
            break;
        case 0x58:
            m_functions[op] = &CM6800::aslb;
            break;
        case 0x68:
        case 0x78:
            m_functions[op] = &CM6800::asl;
            break;
        case 0x47:
            m_functions[op] = &CM6800::asra;
            break;
        case 0x57:
            m_functions[op] = &CM6800::asrb;
            break;
        case 0x67:
        case 0x77:
            m_functions[op] = &CM6800::asr;
            break;
        case 0x44:
            m_functions[op] = &CM6800::lsra;
            break;
        case 0x54:
            m_functions[op] = &CM6800::lsrb;
            break;
        case 0x64:
        case 0x74:
            m_functions[op] = &CM6800::lsr;
            break;
        case 0x97:
        case 0xa7:
        case 0xb7:
            m_functions[op] = &CM6800::staa;
            break;
        case 0xd7:
        case 0xe7:
        case 0xf7:
            m_functions[op] = &CM6800::stab;
            break;
        case 0x80:
        case 0x90:
        case 0xa0:
        case 0xb0:
            m_functions[op] = &CM6800::suba;
            break;
        case 0xc0:
        case 0xd0:
        case 0xe0:
        case 0xf0:
            m_functions[op] = &CM6800::subb;
            break;
        case 0x10:
            m_functions[op] = &CM6800::sba;
            break;
        case 0x82:
        case 0x92:
        case 0xa2:
        case 0xb2:
            m_functions[op] = &CM6800::sbca;
            break;
        case 0xc2:
        case 0xd2:
        case 0xe2:
        case 0xf2:
            m_functions[op] = &CM6800::sbcb;
            break;
        case 0x16:
            m_functions[op] = &CM6800::tab;
            break;
        case 0x17:
            m_functions[op] = &CM6800::tba;
            break;
        case 0x4d:
            m_functions[op] = &CM6800::tsta;
            break;
        case 0x5d:
            m_functions[op] = &CM6800::tstb;
            break;
        case 0x6d:
        case 0x7d:
            m_functions[op] = &CM6800::tst;
            break;
        // BRANCH OPERATIONS
        case 0x20:
            m_functions[op] = &CM6800::bra;
            break;
        case 0x22:
            m_functions[op] = &CM6800::bhi;
            break;
        case 0x23:
            m_functions[op] = &CM6800::bls;
            break;
        case 0x24:
            m_functions[op] = &CM6800::bcc;
            break;
        case 0x25:
            m_functions[op] = &CM6800::bcs;
            break;
        case 0x26:
            m_functions[op] = &CM6800::bne;
            break;
        case 0x27:
            m_functions[op] = &CM6800::beq;
            break;
        case 0x28:
            m_functions[op] = &CM6800::bvc;
            break;
        case 0x29:
            m_functions[op] = &CM6800::bvs;
            break;
        case 0x2a:
            m_functions[op] = &CM6800::bpl;
            break;
        case 0x2b:
            m_functions[op] = &CM6800::bmi;
            break;
        case 0x2c:
            m_functions[op] = &CM6800::bge;
            break;
        case 0x2d:
            m_functions[op] = &CM6800::blt;
            break;
        case 0x2e:
            m_functions[op] = &CM6800::bgt;
            break;
        case 0x2f:
            m_functions[op] = &CM6800::ble;
            break;
        case 0x8d:
            m_functions[op] = &CM6800::bsr;
            break;
        case 0xad:
        case 0xbd:
            m_functions[op] = &CM6800::jsr;
            break;
        case 0x6e:
        case 0x7e:
            m_functions[op] = &CM6800::jmp;
            break;
        case 0x01:
            m_functions[op] = &CM6800::nop;
            break;
        case 0x39:
            m_functions[op] = &CM6800::rts;
            break;
        case 0x3b:
            m_functions[op] = &CM6800::rti;
            break;
        case 0x3e:
            m_functions[op] = &CM6800::wai;
            break;
        case 0x3f:
            m_functions[op] = &CM6800::swi;
            break;
        // INDEX & STACK
        case 0x8c:
        case 0x9c:
        case 0xac:
        case 0xbc:
            m_functions[op] = &CM6800::cpx;
            break;
        case 0x09:
            m_functions[op] = &CM6800::dex;
            break;
        case 0x34:
            m_functions[op] = &CM6800::des;
            break;
        case 0x08:
            m_functions[op] = &CM6800::inx;
            break;
        case 0x31:
            m_functions[op] = &CM6800::ins;
            break;
        case 0xce:
        case 0xde:
        case 0xee:
        case 0xfe:
            m_functions[op] = &CM6800::ldx;
            break;
        case 0x8e:
        case 0x9e:
        case 0xae:
        case 0xbe:
            m_functions[op] = &CM6800::lds;
            break;
        case 0xdf:
        case 0xef:
        case 0xff:
            m_functions[op] = &CM6800::stx;
            break;
        case 0x9f:
        case 0xaf:
        case 0xbf:
            m_functions[op] = &CM6800::sts;
            break;
        case 0x35:
            m_functions[op] = &CM6800::txs;
            break;
        case 0x30:
            m_functions[op] = &CM6800::tsx;
            break;
        // CCR OPS
        case 0x0c:
            m_functions[op] = &CM6800::clc;
            break;
        case 0x0d:
            m_functions[op] = &CM6800::sec;
            break;
        case 0x0a:
            m_functions[op] = &CM6800::clv;
            break;
        case 0x0b:
            m_functions[op] = &CM6800::sev;
            break;
        case 0x0e:
            m_functions[op] = &CM6800::cli;
            break;
        case 0x0f:
            m_functions[op] = &CM6800::sei;
            break;
        case 0x06:
            m_functions[op] = &CM6800::tap;
            break;
        case 0x07:
            m_functions[op] = &CM6800::tpa;
            break;
        default:
            m_functions[op] = nullptr;
        }
    }
}
