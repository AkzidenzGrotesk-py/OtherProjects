
       id division.
       program-id. controller.

       data division.
           working-storage section.
      *    player positions
           01 WS-PX     pic 999 value 0.
           01 WS-PY     pic 999 value 0.

      *    editable edges of playable area
           01 WS-WIDTH  pic 99 value 20.
           01 WS-HEIGHT pic 99 value 20.

      *    temporary
           01 WS-J      pic 99 value 0.
           01 WS-I      pic 99 value 0.
           01 WS-INPUT  pic X value " ".

      *    loop
           01 WS-ACTIVE pic 9 value 0.

       procedure division.
           PARA-ENTRY.
               call "SYSTEM" using "title Controller".
               perform PARA-LOOP with test after until WS-ACTIVE = 1.
               go to PARA-EXIT.

           PARA-EXIT.
               call "SYSTEM" using "pause".
               stop run.

           PARA-LOOP.
               call "SYSTEM" using "cls".
      * moved line to stop overflow over edge -->
               display "*" with no advancing.
       perform PARA-RENDER-LINE with test after until WS-I=WS-WIDTH.
               move 0 to WS-I.
               display "*".
       perform PARA-RENDER-LINES with test after until WS-J=WS-HEIGHT.
               move 0 to WS-J WS-I.
               display "*" with no advancing.
       perform PARA-RENDER-LINE with test after until WS-I=WS-WIDTH.
               move 0 to WS-I.
               display "*".

               display "> " with no advancing.
               accept WS-INPUT.
               move function lower-case(WS-INPUT) to WS-INPUT.

               subtract 1 from WS-HEIGHT WS-WIDTH.
               evaluate true
                   when WS-INPUT = "w" and WS-PY > 0
                       subtract 1 from WS-PY

                   when WS-INPUT = "s" and WS-PY < WS-HEIGHT
                       add 1 to WS-PY

                   when WS-INPUT = "a" and WS-PX > 0
                       subtract 1 from WS-PX

                   when WS-INPUT = "d" and WS-PX < WS-WIDTH
                       add 1 to WS-PX

                   when WS-INPUT = "q"
                       move 1 to WS-ACTIVE

                   when other
                       display 'INVALID INPUT.'
               end-evaluate.
               add 1 to WS-HEIGHT WS-WIDTH.

           PARA-RENDER-LINES.
      * moved line to stop overflow over edge -->
               display "|" with no advancing.
       perform PARA-RENDER-CELLS with test after until WS-I=WS-WIDTH.
               display "|".
               add 1 to WS-J.
               move 0 to WS-I.

           PARA-RENDER-CELLS.
               if WS-J = WS-PY and WS-I = WS-PX
                   display "* " with no advancing
               else
                   display "  " with no advancing
               end-if.
               add 1 to WS-I.

           PARA-RENDER-LINE.
               display "--" with no advancing.
               add 1 to WS-I.
