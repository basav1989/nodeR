{
  "variables": {
    "CURRENT_DIR": '<!(pwd)',
    "R_CUSTOM_PACKAGES": '/home/performance/R/x86_64-pc-linux-gnu-library/3.3'
  },
  "targets": [
    {
      "target_name": "libRbinding",
      "type": "none",
      "actions": [
        {
          "action_name": "libRbinding",
          "inputs": [],
          "outputs": ["Rbindings"],
          "action": ["make", "-C", "<@(CURRENT_DIR)/../Rbindings/"]
        }
      ]
    },
    {
      "target_name": "volatilityModel",
      "cflags": ["-std=c++11", "-g -O2 -fstack-protector -fexceptions --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2 -g -Wall  -Wl,--export-dynamic -fopenmp"],
      "cflags_cc!": [ '-fno-rtti', '-fno-exceptions' ],
      'cflags!': [ '-fno-exceptions' ],
      "include_dirs": [
        '/usr/share/R/include', '<@(R_CUSTOM_PACKAGES)/Rcpp/include', '<@(R_CUSTOM_PACKAGES)/RInside/include', '<@(CURRENT_DIR)/../Rbindings'],
      "link_settings": {
        'libraries': ['-L/usr/lib/R/lib', '-lR -lpcre -llzma -lbz2 -lz -lrt -ldl -lm  -lblas -llapack', '-L<@(R_CUSTOM_PACKAGES)/RInside/lib', '-lRInside', '-Wl,-rpath,<@(R_CUSTOM_PACKAGES)/RInside/lib -L<@(CURRENT_DIR)/../Rbindings/release -lVolatilityModel', '-Wl,-rpath,<@(CURRENT_DIR)/../Rbindings/release'],
      },
      "sources": [ "volatilityModel.cc" ],
    }
  ]
}
