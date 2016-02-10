/*
debugUtils.h - Simple debugging utilities.
*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H


    #ifdef USE_DEBUG
        #ifdef ESP8266
                #include <ESP8266WiFi.h>
                #ifndef SERIAL_DEBUG_SEPARATOR
                    #define SERIAL_DEBUG_SEPARATOR " "

                #endif // SERIAL_DEBUG_SEPARATOR
                #ifndef SERIAL_DEBUG_PREFIX
                    #define SERIAL_DEBUG_PREFIX "RGB "

                #endif // SERIAL_DEBUG_PREFIX
                #define DEBUG_0()                   Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.println("### SerialDebug ###")

                #define DEBUG_1(A)                  Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.println(A)
                #define DEBUG_2(A,B)                Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(B)
                #define DEBUG_3(A,B,C)              Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(C)
                #define DEBUG_4(A,B,C,D)            Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(D)
                #define DEBUG_5(A,B,C,D,E)          Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(D);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(E)
                #define DEBUG_6(A,B,C,D,E,F)        Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(D);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(E);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(F)
                #define DEBUG_7(A,B,C,D,E,F,G)      Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(D);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(E);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(F);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(G)
                #define DEBUG_8(A,B,C,D,E,F,G,H)    Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(D);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(E);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(F);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(G);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(H)
                #define DEBUG_9(A,B,C,D,E,F,G,H,I)  Serial.print(SERIAL_DEBUG_PREFIX);\
                                                    Serial.print(A);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(B);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(C);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(D);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(E);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(F);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(G);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.print(H);Serial.print(SERIAL_DEBUG_SEPARATOR);\
                                                    Serial.println(I)

                #define DEBUG_X(x,A,B,C,D,E,F,G,H,I,MACRO, ...) MACRO

                #define DEBUG(...)                  DEBUG_X(,##__VA_ARGS__,\
                                                        DEBUG_9(__VA_ARGS__),\
                                                        DEBUG_8(__VA_ARGS__),\
                                                        DEBUG_7(__VA_ARGS__),\
                                                        DEBUG_6(__VA_ARGS__),\
                                                        DEBUG_5(__VA_ARGS__),\
                                                        DEBUG_4(__VA_ARGS__),\
                                                        DEBUG_3(__VA_ARGS__),\
                                                        DEBUG_2(__VA_ARGS__),\
                                                        DEBUG_1(__VA_ARGS__),\
                                                        DEBUG_0(__VA_ARGS__)\
                                                    )

                #define SERIAL_DEBUG_SETUP(SPEED)   Serial.begin(SPEED)
            #else
                #include <iostream>
                #ifndef DEBUG_SEPARATOR
                    #define DEBUG_SEPARATOR " "

                #endif // DEBUG_SEPARATOR
                #ifndef DEBUG_PREFIX
                    #define DEBUG_PREFIX "RGB "

                #endif

                #define DEBUG_0()                   (std::cout << DEBUG_PREFIX << "### ConsoleDebug ###" << std::endl)
                #define DEBUG_1(A)                  (std::cout << DEBUG_PREFIX <<  A <<std::endl)
                #define DEBUG_2(A,B)                (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B << std::endl)
                #define DEBUG_3(A,B,C)              (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C <<std::endl;)
                #define DEBUG_4(A,B,C,D)            (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D << std::endl)
                #define DEBUG_5(A,B,C,D,E)          (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D \
                                                     << DEBUG_SEPARATOR << E << std::endl;)
                #define DEBUG_6(A,B,C,D,E,F)        (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D \
                                                     << DEBUG_SEPARATOR << E << DEBUG_SEPARATOR << F << std::endl)
                #define DEBUG_7(A,B,C,D,E,F,G)      (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D \
                                                     << DEBUG_SEPARATOR << E << DEBUG_SEPARATOR << F \
                                                     << DEBUG_SEPARATOR << G << std::endl;)
                #define DEBUG_8(A,B,C,D,E,F,G,H)    (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D \
                                                     << DEBUG_SEPARATOR << E << DEBUG_SEPARATOR << F \
                                                     << DEBUG_SEPARATOR << G << DEBUG_SEPARATOR << H << std::endl)
                #define DEBUG_9(A,B,C,D,E,F,G,H,I)  (std::cout << DEBUG_PREFIX <<  A << DEBUG_SEPARATOR << B \
                                                     << DEBUG_SEPARATOR << C << DEBUG_SEPARATOR << D \
                                                     << DEBUG_SEPARATOR << E << DEBUG_SEPARATOR << F \
                                                     << DEBUG_SEPARATOR << G << DEBUG_SEPARATOR << H \
                                                     << DEBUG_SEPARATOR << I << std::endl)

                #define DEBUG_X(x,A,B,C,D,E,F,G,H,I,MACRO, ...) MACRO

                #define DEBUG(...)                  DEBUG_X(,##__VA_ARGS__,\
                                                        DEBUG_9(__VA_ARGS__),\
                                                        DEBUG_8(__VA_ARGS__),\
                                                        DEBUG_7(__VA_ARGS__),\
                                                        DEBUG_6(__VA_ARGS__),\
                                                        DEBUG_5(__VA_ARGS__),\
                                                        DEBUG_4(__VA_ARGS__),\
                                                        DEBUG_3(__VA_ARGS__),\
                                                        DEBUG_2(__VA_ARGS__),\
                                                        DEBUG_1(__VA_ARGS__),\
                                                        DEBUG_0(__VA_ARGS__)\
                                                    )


                #define SERIAL_DEBUG_SETUP(SPEED)
            #endif // ESP8266
        #else
            #define DEBUG(...)
            #define SERIAL_DEBUG_SETUP(SPEED)
        #endif // DEBUG

#endif
