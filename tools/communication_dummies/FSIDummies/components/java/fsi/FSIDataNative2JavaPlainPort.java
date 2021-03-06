//
// ASCoDT - Advanced Scientific Computing Development Toolkit
//
// This file was generated by ASCoDT's simplified SIDL compiler.
//
// Authors: Tobias Weinzierl, Atanas Atanasov   
//
package fsi;


public class FSIDataNative2JavaPlainPort extends FSIDataAbstractPort {

  private long _ref;
  public FSIDataNative2JavaPlainPort() {
    super();
    createInstance();
  }
  
  public native void createInstance();
  public native void destroyInstance(long ref);
  /**
   * Disconnect a port.
   * 
   * @throw If already not connected to type port.
   * @see Operation with name of any implementing component 
   */
  public void disconnect(FSIData  port) throws de.tum.ascodt.utils.exceptions.ASCoDTException {
     super.disconnect(port);
  }
  
  public void destroy(){
     destroyInstance(_ref);    
  }
  public long getReference(){
    return _ref;
  }  
  
  public void setReference(long ref){
     _ref=ref;
  }

  public void transferData(final int coordId[],final double data[]) {
   // if (_destination!=null) {
      //_destination.lock();
      _destination.transferData(coordId,data);
      //_destination.unlock();
    //}
  }
  
  public void transferDataParallel(final int coordId[],final double data[]) {
   // if (_destination!=null) {
      //_destination.lock();
      _destination.transferDataParallel(coordId,data);
      //_destination.unlock();
    //}
  }
  
  
  

  public void dataAck(int ack[]) {
   // if (_destination!=null) {
      //_destination.lock();
      _destination.dataAck(ack);
      //_destination.unlock();
    //}
  }
  
  public void dataAckParallel(int ack[]) {
   // if (_destination!=null) {
      //_destination.lock();
      _destination.dataAckParallel(ack);
      //_destination.unlock();
    //}
  }
  
  
  


}
 


