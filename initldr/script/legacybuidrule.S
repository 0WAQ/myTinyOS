###################################################################
#		krnlbuidrule自动化编译配置文件 Makefile		  #
###################################################################

CCSTR		= 	'CC -[M] 正在构建... '$<
PRINTCSTR 	=	@echo $(CCSTR) 

%.o : $(CCBUILDPATH)%.16asm
[$(ASM) $(ASMBFLAGS) -o $@ $<
[@echo 'AS -[M] 正在构建...' $<
%.o : $(CCBUILDPATH)%.asm
[$(ASM) $(ASMBFLAGS) -o $@ $<
[@echo 'AS -[M] 正在构建...' $<
%.o : $(CCBUILDPATH)%.c
[$(CC) $(BTCFLAGS) -o $@ $<
[$(PRINTCSTR) 
%.lds : $(CCBUILDPATH)%.S
[$(CC) $(CPPFLGSLDS) -o $@ $<
[$(PRINTCSTR)

%.mh : $(CCBUILDPATH)%.S
[$(CC) $(CPPFLGSLDS) -o $@ $<
[$(PRINTCSTR)

%.lib : $(CCBUILDPATH)%.c
[$(CC) $(CFLAGS) -o $@ $<
[$(PRINTCSTR)
%.lib : $(CCBUILDPATH)%.asm
[$(ASM) $(ASMKFLAGS) -o $@ $<
[@echo 'AS -[M] 正在构建...' $< 
