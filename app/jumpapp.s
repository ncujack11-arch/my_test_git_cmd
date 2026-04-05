
                AREA    |.text|, CODE, READONLY
                PRESERVE8
                THUMB


                EXPORT  JumpApp


JumpApp         PROC
                LDR     SP, [R0, #0]

                LDR     PC, [R0, #4]

                ENDP

                END
