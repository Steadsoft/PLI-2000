
    dcl 1 plh                               based (plh_ptr) volatile,
          2 processes,
            3 number                        bin (15),
          2 first_ple                       pointer;

