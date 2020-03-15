
void IntSet(int64_t * dest, int64_t * src);		// This initializes uninitialized integer
void IntInit(int64_t * dest, int32_t n);
void IntSetMin(int64_t * dest);
void IntSetMax(int64_t * dest);

int32_t IntN(int64_t * n);

int64_t * IntMin(int64_t * l, int64_t * r);
int64_t * IntMax(int64_t * l, int64_t * r);


void IntModify(int64_t * dest, int64_t * l);

int64_t * Int0();
int64_t * Int1();
int64_t * Int255();

void IntAdd(int64_t * dest, int64_t * l, int64_t * r);
void IntSub(int64_t * dest, int64_t * l, int64_t * r);
void IntMul(int64_t * dest, int64_t * l, int64_t * r);
void IntDiv(int64_t * dest, int64_t * l, int64_t * r);
void IntMod(int64_t * dest, int64_t * l, int64_t * r);
void IntShr(int64_t * dest, int64_t * l, int64_t * r);

void IntAnd(int64_t * dest, int64_t * l, int64_t * r);
void IntOr(int64_t * dest, int64_t * l, int64_t * r);
void IntXor(int64_t * dest, int64_t * l, int64_t * r);

void IntSqrt(int64_t * dest, int64_t * l);
void IntNeg(int64_t * dest);

bool IntEq(int64_t * l, int64_t * r);
bool IntLower(int64_t * l, int64_t * r);
bool IntHigher(int64_t * l, int64_t * r);
bool IntLowerEq(int64_t * l, int64_t * r);
bool IntHigherEq(int64_t * l, int64_t * r);

bool IntEqN(int64_t * l, int32_t N);
bool IntHigherN(int64_t * l, int32_t N);
bool IntLowerN(int64_t * l, int32_t N);
bool IntLowerEqN(int64_t * l, int32_t N); 

void IntMulN(int64_t * dest, int32_t N);
void IntDivN(int64_t * dest, int32_t N);
void IntAddN(int64_t * dest, int32_t N);
void IntAndN(int64_t * dest, uint32_t N);
void IntOrN(int64_t * dest, uint32_t N);

void Printint64_t(int64_t * n);

void IntFree(int64_t * n);

//----- Generic

void IntRangeSize(int64_t * dest, int64_t * min, int64_t * max);
// max - min + 1
