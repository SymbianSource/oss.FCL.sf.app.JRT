This test set tests the coreui framework - both native and Java.

Precoditions:
    -This test will override the javacoreui.dll. This requirtes that before
     compiling the test set run clean first.
    -Compile the test sets from the root of the tsrc.

To run test start eshell and run tst.bat. Then remove the javacoreui.dll from
epoc32\release\winscw\udeb\ directory and run tst2.bat.

After tests recompile coreui_akn or coreui_qt.