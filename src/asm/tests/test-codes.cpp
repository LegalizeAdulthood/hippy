#include <codes.h>
#include <lexer.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>

class TestInstruction : public testing::TestWithParam<Instruction>
{
};

class TestInherent : public TestInstruction
{
};

TEST_P(TestInherent, isInherent)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icInher);
}

class TestImmediate : public TestInstruction
{
};

TEST_P(TestImmediate, isImmediate)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icImmed);
}

class TestRelative : public TestInstruction
{
};

TEST_P(TestRelative, isRelative)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icRel);
}

class TestDirect : public TestInstruction
{
};

TEST_P(TestDirect, isDirect)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icDirect);
}

class TestIndexed : public TestInstruction
{
};

TEST_P(TestIndexed, isIndexed)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icIndex);
}

class TestExtended : public TestInstruction
{
};

TEST_P(TestExtended, isExtended)
{
    EXPECT_NE(0x00, instCodes[GetParam()].icExtend);
};

static Instruction inherentInstructions[] = {
    ABA,  ASLA, ASLB, ASRA, ASRB, CBA,  CLC,  CLI,  CLRA, CLRB, CLV,  COMA, COMB, DAA,  DECA, DECB, DES,
    DEX,  INCA, INCB, INS,  INX,  LSRA, LSRB, NEGA, NEGB, NOP,  PSHA, PSHB, PULA, PULB, ROLA, ROLB, RORA,
    RORB, RTI,  RTS,  SBA,  SEC,  SEI,  SEV,  SWI,  TAB,  TAP,  TBA,  TPA,  TSTA, TSTB, TSX,  TXS,  WAI,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestInherent, testing::ValuesIn(inherentInstructions));

static Instruction immediateInstructions[] = {
    ADCA, ADCB, ADDA, ADDB, ANDA, ANDB, BITA, BITB, CMPA, CMPB, CPX,  EORA,
    EORB, LDAA, LDAB, LDS,  LDX,  ORAA, ORAB, SBCA, SBCB, SUBA, SUBB,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestImmediate, testing::ValuesIn(immediateInstructions));

static Instruction relativeInstructions[] = {
    BCC, BCS, BEQ, BGE, BGT, BHI, BLE, BLS, BLT, BMI, BNE, BPL, BRA, BSR, BVC, BVS,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestRelative, testing::ValuesIn(relativeInstructions));

static Instruction directInstructions[] = {
    ADCA, ADCB, ADDA, ADDB, ANDA, ANDB, BITA, BITB, CMPA, CMPB, CPX, EORA, EORB, LDAA,
    LDAB, LDS,  LDX,  ORAA, ORAB, SBCA, SBCB, STAA, STAB, STS,  STX, SUBA, SUBB,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestDirect, testing::ValuesIn(directInstructions));

static Instruction indexedInstructions[] = {
    ADCA, ADCB, ADDA, ADDB, ANDA, ANDB, ASL, ASR,  BITA, BITB, CLR, CMPA, CMPB, COM,  CPX,  DEC, EORA, EORB, INC,  JMP,
    JSR,  LDAA, LDAB, LDS,  LDX,  LSR,  NEG, ORAA, ORAB, ROL,  ROR, SBCA, SBCB, STAA, STAB, STS, STX,  SUBA, SUBB, TST,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestIndexed, testing::ValuesIn(indexedInstructions));

static Instruction extendedInstructions[] = {
    ADCA, ADCB, ADDA, ADDB, ANDA, ANDB, ASL, ASR,  BITA, BITB, CLR, CMPA, CMPB, COM,  CPX,  DEC, EORA, EORB, INC,  JMP,
    JSR,  LDAA, LDAB, LDS,  LDX,  LSR,  NEG, ORAA, ORAB, ROL,  ROR, SBCA, SBCB, STAA, STAB, STS, STX,  SUBA, SUBB, TST,
};

INSTANTIATE_TEST_SUITE_P(TestInstructions, TestExtended, testing::ValuesIn(extendedInstructions));

TEST(TestInstructions, notInherent)
{
    const auto end = std::end(inherentInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(inherentInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icInher) << "instruction " << ins;
        }
    }
}

TEST(TestInstructions, notImmediate)
{
    const auto end = std::end(immediateInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(immediateInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icImmed) << "instruction " << ins;
        }
    }
}

TEST(TestInstructions, notRelative)
{
    const auto end = std::end(relativeInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(relativeInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icRel) << "instruction " << ins;
        }
    }
}

TEST(TestInstructions, notDirect)
{
    const auto end = std::end(directInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(directInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icDirect) << "instruction " << ins;
        }
    }
}

TEST(TestInstructions, notIndexed)
{
    const auto end = std::end(indexedInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(indexedInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icIndex) << "instruction " << ins;
        }
    }
}

TEST(TestInstructions, notExtended)
{
    const auto end = std::end(extendedInstructions);
    for (int ins = 0; ins < 107; ++ins)
    {
        if (std::find(std::begin(extendedInstructions), end, ins) == end)
        {
            EXPECT_EQ(0x00, instCodes[ins].icExtend) << "instruction " << ins;
        }
    }
}
