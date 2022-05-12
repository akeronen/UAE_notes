
/* If char has more then 8 bits, good night. */
typedef unsigned char uae_u8;
typedef signed char uae_s8;

typedef unsigned int uae_u32;
typedef int uae_s32;

#define IMM uae_u32
#define R1  uae_u32
#define R2  uae_u32
#define R4  uae_u32
#define W1  uae_u32
#define W2  uae_u32
#define W4  uae_u32
#define RW1 uae_u32
#define RW2 uae_u32
#define RW4 uae_u32
#define MEMR uae_u32
#define MEMW uae_u32
#define MEMRW uae_u32

#define MIDFUNC(nargs,func,args) void func args
#define MENDFUNC(nargs,func,args)
#define COMPCALL(func) func

#define LOWFUNC(flags,mem,nargs,func,args) static __inline__ void func args
#define LENDFUNC(flags,mem,nargs,func,args)


#define CLOBBER_BT   clobber_flags()


static int
readreg(int r, int size)
{
	//return readreg_general(r, size, -1, 0);
	return 0;
}

static __inline__ void
clobber_flags(void)
{
	//if (live.flags_in_flags == VALID && live.flags_on_stack != VALID)
	//	flags_to_stack();
	//live.flags_in_flags = TRASH;
}


static uae_u8 *target;

static __inline__ void
emit_byte(uae_u8 x)
{
	*target++ = x;
}


static void
unlock2(int r)
{
	//Dif(!live.nat[r].locked) abort();
	//live.nat[r].locked--;
}

static void
make_flags_live_internal(void)
{
	/*
	if (live.flags_in_flags == VALID)
		return;
	Dif(live.flags_on_stack == TRASH)
	{
		write_log("Want flags, got something on stack, but it is TRASH\n");
		abort();
	}
	if (live.flags_on_stack == VALID)
	{
		int tmp;

		tmp = readreg_specific(FLAGTMP, 4, FLAG_NREG2);
		raw_reg_to_flags(tmp);
		unlock2(tmp);

		live.flags_in_flags = VALID;
		return;
	}
	write_log
		("Huh? live.flags_in_flags=%d, live.flags_on_stack=%d, but need to make live\n",
		live.flags_in_flags, live.flags_on_stack);
	abort();
	*/
}

// INTERESTED ONES

LOWFUNC(WRITE,NONE,2,raw_bt_l_rr,(R4 r, R4 b))
{
	emit_byte(0x0f);
	emit_byte(0xa3);
	emit_byte(0xc0+8*b+r);
}
LENDFUNC(WRITE,NONE,2,raw_bt_l_rr,(R4 r, R4 b))


LOWFUNC(WRITE,NONE,2,raw_bt_l_ri,(R4 r, IMM i))
{
	emit_byte(0x0f);
	emit_byte(0xba);
	emit_byte(0xe0+r);
	emit_byte(i);
}
LENDFUNC(WRITE,NONE,2,raw_bt_l_ri,(R4 r, IMM i))


MENDFUNC(2, bt_l_ri, (R4 r, IMM i))	/* This is defined as only affecting C */
	MIDFUNC(2, bt_l_rr, (R4 r, R4 b))/* This is defined as only affecting C */
{
	CLOBBER_BT;
	r = readreg(r, 4);
	b = readreg(b, 4);
	raw_bt_l_rr(r, b);
	unlock2(r);
	unlock2(b);
}

// SUPER INTERESTED ONES

static void
fflags_into_flags_internal(uae_u32 tmp)
{
	int r;

	clobber_flags();
	//r = f_readreg(FP_RESULT);
	//raw_fflags_into_flags(r);
	//f_unlock(r);
}

uae_u32 needflags = 0;

MENDFUNC(0, restore_carry, (void)) MIDFUNC(0, start_needflags, (void))
{
	needflags = 1;
}

MENDFUNC(0, start_needflags, (void)) MIDFUNC(0, end_needflags, (void))
{
	needflags = 0;
}

MENDFUNC(0, end_needflags, (void)) MIDFUNC(0, make_flags_live, (void))
{
	make_flags_live_internal();
}

MENDFUNC(0, make_flags_live, (void)) MIDFUNC(1, fflags_into_flags, (W2 tmp))
{
	clobber_flags();
	fflags_into_flags_internal(tmp);
}

MENDFUNC(1, fflags_into_flags, (W2 tmp)) MIDFUNC(2, bt_l_ri, (R4 r, IMM i))	/* This is defined as only affecting C */
{
	int size = 4;

	if (i < 16)
		size = 2;
	CLOBBER_BT;
	r = readreg(r, size);
	raw_bt_l_ri(r, i);
	unlock2(r);
}

