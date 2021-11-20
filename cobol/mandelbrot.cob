       id division.
       program-id. mandelbrot.

       data division.
           working-storage section.

      * change width and height values for more resolution
           01 WIDTH       pic 999 value 50.
           01 HEIGHT      pic 999 value 50.
           01 MAX_ITER    pic 999 value 150.
           01 MAX_ITER-1  pic 999 value 149.
           01 C_X         pic 999 value 1.
           01 C_Y         pic 999 value 1.
           01 C_N         pic 999 value 0.
           01 G_A         pic S9999V9999.
           01 G_B         pic S9999V9999.
           01 G_AA        pic S9999V9999.
           01 G_BB        pic S9999V9999.
           01 G_CA        pic S9999V9999.
           01 G_CB        pic S9999V9999.
           01 G_TEMP      pic S9999V9999.
           01 G_Z         pic S9999V9999.
           01 G_BRIGHT    pic S9999V9999.

       procedure division.
           P0000-MAIN.
           display " ".
           display " ".
           display " ".
           display " ".
           move 1 to C_X.
           perform until C_X is equal to WIDTH
               display "        " with no advancing
               move 1 to C_Y
               perform until C_Y is equal to HEIGHT
                   compute G_B = (C_X - 0) / (WIDTH - 0) * 4 - 2
                   compute G_A = (C_Y - 0) / (HEIGHT - 0) * 4 - 2
                   move G_A to G_CA
                   move G_B to G_CB

                   move 0 to G_Z
                   move 0 to C_N
                   perform until C_N is equal to MAX_ITER
                       compute G_AA = G_A * G_A - G_B * G_B
                       compute G_BB = 2 * G_A * G_B
                       compute G_A = G_AA + G_CA
                       compute G_B = G_BB + G_CB

                       compute G_TEMP = G_A + G_B
                       if G_TEMP > 16
                           exit perform
                       end-if
                       move C_N to G_Z

                       add 1 to C_N
                   end-perform

                   compute G_BRIGHT = (G_Z - 0) / (MAX_ITER - 0) * 255
                   if MAX_ITER-1 = G_Z
                       move 0 to G_BRIGHT
                   end-if

                   evaluate true
                       when G_BRIGHT > 24
                           display "@@" with no advancing
                       when G_BRIGHT > 20
                           display "%@" with no advancing
                       when G_BRIGHT > 18
                           display "%%" with no advancing
                       when G_BRIGHT > 16
                           display "#%" with no advancing
                       when G_BRIGHT > 14
                           display "##" with no advancing
                       when G_BRIGHT > 12
                           display "*#" with no advancing
                       when G_BRIGHT > 10
                           display "**" with no advancing
                       when G_BRIGHT > 9
                           display "+*" with no advancing
                       when G_BRIGHT > 8
                           display "++" with no advancing
                       when G_BRIGHT > 7
                           display "=+" with no advancing
                       when G_BRIGHT > 6
                           display "==" with no advancing
                       when G_BRIGHT > 5
                           display "-=" with no advancing
                       when G_BRIGHT > 4
                           display "--" with no advancing
                       when G_BRIGHT > 3
                           display ".-" with no advancing
                       when G_BRIGHT > 2
                           display ".." with no advancing
                       when G_BRIGHT > 1
                           display ". " with no advancing
                       when other
                           display "  " with no advancing
                   end-evaluate

                   add 1 to C_Y
               end-perform
               display " "
               add 1 to C_X
           end-perform.
           display " ".
           display " ".
           display " ".
           display " ".

           P9999-EXIT.
           stop run.
