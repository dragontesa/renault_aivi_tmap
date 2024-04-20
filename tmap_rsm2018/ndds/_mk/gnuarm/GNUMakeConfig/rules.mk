ifdef PCH_H
$(PCH):$(PCH_H)
	@echo -e '\e[1;34mPrepare precompiled header $@...\e[0m'
	@$(CC) $(CFLAGS) -x c-header -c -o $@ $<
endif
ifdef PCH_CXX_H
$(PCH_CXX):$(PCH_CXX_H)
	@echo -e '\e[1;34mPrepare precompiled header $@...\e[0m'
	@$(CXX) $(CXXFLAGS) -x c++-header -c -o $@ $<
endif

ifeq ($(OUTDIR),)
%.o : %.c
	$(COMPILE_C)

%.o : %.cc
	$(COMPILE_CXX)

%.o : %.cpp
	$(COMPILE_CXX)

%.o : %.c++
	$(COMPILE_CXX)

%.o : %.cxx
	$(COMPILE_CXX)
else
$(OUTDIR)/%.o:%.c
	$(COMPILE_C)

$(OUTDIR)/%.o:%.cc
	$(COMPILE_CXX)

$(OUTDIR)/%.o:%.cpp
	$(COMPILE_CXX)

$(OUTDIR)/%.o:%.c++
	$(COMPILE_CXX)

$(OUTDIR)/%.o:%.cxx
	$(COMPILE_CXX)
endif

ifneq ($(DEPS),)  
  -include $(DEPS)  
endif  

objs:$(OBJS)

tags: $(HEADERS) $(SRCS)  
	$(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SRCS)  
  
ctags: $(HEADERS) $(SRCS)  
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SRCS)

help:  
	@echo 'Makefile template for C/C++ Programs'  
	@echo 'Autor: pffang<pffang@iflytek.com>'  
	@echo  
	@echo 'Usage: make [TARGET]'  
	@echo 'TARGETS:'  
	@echo '  all	   (=make) compile and link.'  
	@echo '  objs	  compile only (no linking).'  
	@echo '  tags	  create tags for Emacs editor.'  
	@echo '  ctags	 create ctags for VI editor.'  
	@echo '  clean	 clean objects and the executable file.'  
	@echo '  distclean	 clean objects, the executable, dependencies and all.'  
	@echo '  show	  show variables (for debug use only).'  
	@echo '  help	  print this message.'  
  
# Show variables (for debug use only.)  
show:  
	@echo 'TARGET	 :' $(TARGET)  
	@echo 'SRCDIRS	 :' $(SRCDIRS)  
	@echo 'HEADERS	 :' $(HEADERS)  
	@echo 'SRCS	 :' $(SRCS)
	@echo 'SRCS_C	 :' $(SRCS_C)
	@echo 'SRCS_CXX	 :' $(SRCS_CXX) 
	@echo 'PCH_H	 :' $(PCH_H)
	@echo 'PCH_CXX_H	 :' $(PCH_CXX_H)
	@echo 'PCH	 :' $(PCH)
	@echo 'PCH_CXX	 :' $(PCH_CXX)
	@echo 'OBJS		:' $(OBJS)  
	@echo 'DEPS		:' $(DEPS)  
	@echo 'DEPEND	  :' $(DEPEND)
	@echo 'CC	 :' $(CC)  
	@echo 'CXX	 :' $(CXX)  
	@echo 'AR	 :' $(AR)  
	@echo 'LD	 :' $(LD)
	@echo 'CPPFLAGS	 :' $(CPPFLAGS)
	@echo 'CFLAGS	 :' $(CFLAGS)  
	@echo 'CXXFLAGS		:' $(CXXFLAGS)  
	@echo 'LDFLAGS		:' $(LDFLAGS)
	@echo 'COMPILE_C		:' $(COMPILE_C)  
	@echo 'COMPILE_CXX		:' $(COMPILE_CXX) 	
