#ifndef OTHER_INSTRUCTIONS_H_
#define OTHER_INSTRUCTIONS_H_

#include "instructions.h"

// No Operation instruction
class NOP : public Instruction
{
public:

   NOP ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

// ~~~~~~~~~~~~~~~~~

// halt: stop CPU until an interruption arrives
class Halt : public Instruction
{
public:

   Halt ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

// ~~~~~~~~~~~~~~~~~

// halt: stop CPU until an interruption arrives
class Stop : public Instruction
{
public:

   Stop ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

// ~~~~~~~~~~~~~~~~~

// enable or disable interrupts (in the next isntruction)
class InterruptionED : public Instruction
{
public:

   InterruptionED (bool enable);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool enable_;
};

// ~~~~~~~~~~~~~~~~~

// Jump instructions (JP and JR) and CALL
class JumpCall : public Instruction
{
public:
   enum Source {r8, a16, HL};
   enum Type {NZ, Z, NC, C, INCONDITIONAL};

   JumpCall (bool call, bool relative, Source source, Type type);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);
   unsigned extraCycleTime () const;
   bool longPathTaken () const;

private:

   bool call_, relative_, alternative_path_taken;
   int alternative_path_cpu_cycles;
   Source source_;
   Type type_;
};

// ~~~~~~~~~~~~~~~~~

// RST, RET and RETI instructions
class Return : public Instruction
{
public:

   enum Source {_00H, _08H, _10H, _18H, _20H, _28H, _30H, _38H, SP};
   enum Type {NZ, Z, NC, C, INCONDITIONAL};

   Return (bool reti, Source source, Type type);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);
   unsigned extraCycleTime () const;
   bool longPathTaken () const;

private:

   bool reti_, jump;
   unsigned jump_cpu_cycles;
   Source source_;
   Type type_;
};

#endif