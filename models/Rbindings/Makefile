SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
R_CUSTOM_PACKAGES = /home/basavaraj.r/R/x86_64-pc-linux-gnu-library/3.4
CPPFLAGS = -g -I/usr/share/R/include -I$(R_CUSTOM_PACKAGES)/Rcpp/include -I$(R_CUSTOM_PACKAGES)/RInside/include -fopenmp -fPIC -std=c++11
LDFLAGS = -g -pthread -Werror=format-security -D_FORTIFY_SOURCE=2 -g -Wall  -Wl,--export-dynamic -fopenmp -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2 -g -Wall  -Wl,--export-dynamic -shared
LDLIBS = -L/usr/lib/R/lib -lR -lpcre -llzma -lbz2 -lz -lrt -ldl -lm  -lblas -llapack  -L$(R_CUSTOM_PACKAGES)/RInside/lib -lRInside -Wl,-rpath,$(R_CUSTOM_PACKAGES)/RInside/lib

libVolatilityModel: volatilityModel.o
	g++ $(LDFLAGS) $(LDLIBS) release/volatilityModel.o -o release/libVolatilityModel.so

volatilityModel.o: volatilityModel.cpp
	mkdir -p release
	g++ $(CPPFLAGS) -std=c++11 -fPIC -c volatilityModel.cpp -o release/volatilityModel.o

clean:
	rm -rf release
