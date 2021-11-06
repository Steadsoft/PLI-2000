/**************************************************************************/
/* Hypothetical include file layout for a structure. Used only for the    */
/* testing of PL/1 compiler.                                              */
/**************************************************************************/

              2 master_name                     char(32) var,
              2 power                           bit(1),
              2 locks,
                3 one                           bin(15),
                3 two                           bin(15),
                3 three                         bin(15),
              2 finally                         bin(31),



        
