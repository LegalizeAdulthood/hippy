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

#ifndef HIPPY_M6800_H
#define HIPPY_M6800_H

#include "addrmng.h"
#include "alu.h"
//#include "disassembler.h"
#include "hippy.h"
#include "stackwnd.h"
#include <afx.h>
#include <afxwin.h>

class CM6800;
class CStackWnd;

using FnType = void (CM6800::*)();

class CM6800
{

public:
    CM6800(CStackWnd *psw);
    ~CM6800();

    void SetRunMode(int mode)
    {
        m_runMode = mode;
    }
    bool             MemLocChanged(Word &w);
    CAddressManager *GetMemPtr()
    {
        return &m_memory;
    }
    Registers *GetRegsPtr()
    {
        return &m_regs;
    }
    void Step();

private:
    int             m_runMode{};
    CAddressManager m_memory;
    Registers       m_regs;
    ALU            *m_alu;
    Word            m_mar{}; // memory address register
    BYTE            m_mdr{}; // memory data register
    bool            m_done{};
    bool            m_memLocChanged{};
    CSemaphore     *m_irq;
    CSemaphore     *m_nmi;
    CSemaphore     *m_reset;
    CStackWnd      *m_stackWnd{};
    FnType          m_functions[256]{};
    BYTE            m_opcode{};

    void saveState();
    void initFunctions();

    /////// INSTRUCTIONS ////
    /////////////////////////
    //////// ACC & MEM //////

    void adda(), addb(), aba();
    void adca(), adcb();
    void anda(), andb();
    void bita(), bitb();
    void clr(), clra(), clrb();
    void cmpa(), cmpb(), cba();
    void com(), coma(), comb();
    void neg(), nega(), negb();
    void daa();
    void dec(), deca(), decb();
    void eora(), eorb();
    void inc(), inca(), incb();
    void ldaa(), ldab(), lda_uni(BYTE &v1);
    void oraa(), orab();
    void psha(), pshb();
    void pula(), pulb();
    void rol(), rola(), rolb();
    void ror(), rora(), rorb();
    void asl(), asla(), aslb();
    void asr(), asra(), asrb();
    void lsr(), lsra(), lsrb();
    void staa(), stab(), sta_uni(BYTE v1);
    void suba(), subb(), sba();
    void sbca(), sbcb();
    void tab(), tba();
    void tst(), tsta(), tstb();

    ////////////////////
    // BRANCH & JUMP

    void bra();
    void bcc(), bcs();
    void bne(), beq();
    void bhi(), bls();
    void bvc(), bvs();
    void bpl(), bmi();
    void bge(), ble(), blt(), bgt();
    void bsr();
    void jmp(), jsr();
    void nop();
    void rti(), rts();
    void swi(), wai();

    ////////////////////
    // IND REG & STK PTR

    void cpx();
    void dex();
    void des();
    void inx();
    void ins();
    void ldx();
    void lds();
    void stx();
    void sts();
    void txs();
    void tsx();

    /////////////////////
    // CCR OPER.

    void clc();
    void sec();
    void clv();
    void sev();
    void cli();
    void sei();
    void tap();
    void tpa();

    /////////////////////
    // INTERRUPTS

    void nmi();
    void irq();
    void reset();
};

#endif
