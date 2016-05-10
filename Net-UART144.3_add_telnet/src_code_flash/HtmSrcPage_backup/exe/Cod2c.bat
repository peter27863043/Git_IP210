@echo off

call exe\Cod2C DataRate.htm
call exe\Cod2C down.htm
call exe\Cod2C errormsg.htm
call exe\Cod2C firmwareUpdate.htm
call exe\Cod2C index.htm
call exe\Cod2C intro.htm
call exe\Cod2C left.htm
call exe\Cod2C login.htm
call exe\Cod2C MISC.htm
call exe\Cod2C PortSet.htm
call exe\Cod2C PortSetF.htm
call exe\Cod2C QoSset.htm

call exe\Cod2C SCAFUNC.htm
call exe\Cod2C SCANO.htm

call exe\Cod2C SCAwait.htm
call exe\Cod2C setDef.htm

call exe\Cod2C setIP.htm
call exe\Cod2C SetNO.htm

call exe\Cod2C SetOK.htm

call exe\Cod2C setPASS.htm
call exe\Cod2C Status.htm
call exe\Cod2C STP.htm
call exe\Cod2C STPDF.htm

call exe\Cod2C STPDFSet.htm
call exe\Cod2C timeout.htm
call exe\Cod2C up.htm

call exe\Cod2C VLANSelect.htm
call exe\Cod2C RegAccess.htm
call exe\Cod2C idle.htm
call exe\Cod2C Broadcast.htm
call exe\Cod2C TagVLANSelect.htm
call exe\Cod2C TagVLANSet.htm
call exe\Cod2C QoSC.htm
call exe\Cod2C QoSD.htm
call exe\Cod2C QoSP.htm
call exe\Cod2C setBACK.htm
call exe\Cod2C VLANFTTH.htm
call exe\Cod2C MAXLEN.htm
call exe\Cod2C SetON.htm

call exe\Cod2C -b arrow.gif
call exe\Cod2C -b arrow_f2.gif
call exe\Cod2C -b bg01.gif
call exe\Cod2C -b check.gif
call exe\Cod2C -b FTTH.gif
call exe\Cod2C -b green.gif
call exe\Cod2C -b logo.gif
call exe\Cod2C -b spacer.gif
call exe\Cod2C -b square.gif
call exe\Cod2C -b xbg.gif
call exe\Cod2C -b xnetbox0.gif

call exe\Cod2C css.css

copy *.c ..\controlWeb /y
copy *.h ..\controlWeb /y
del *.c
del *.h
