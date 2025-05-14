access(all) contract MicrocontrollerTest {
  access(all) let AdminStoragePath: StoragePath

  access(all) var ControlValue: Int64

  // EventSequenceNumber is the sequence number the next emitted event will have
  access(all) var EventSequenceNumber: UInt64

  access(all) event ControlValueChanged(value:Int64, oldValue:Int64, eventSequence:UInt64)

  access(all) resource Admin {
    access(all) fun setControlValue(_ newValue: Int64) {

      if (MicrocontrollerTest.ControlValue == newValue) {
        // only emit an event if the value actually changed
        return
      }
      let oldValue = MicrocontrollerTest.ControlValue
      MicrocontrollerTest.ControlValue = newValue

      // emit and increment the sequence number
      emit ControlValueChanged(value: newValue, oldValue:oldValue, eventSequence:MicrocontrollerTest.EventSequenceNumber)
      MicrocontrollerTest.EventSequenceNumber = MicrocontrollerTest.EventSequenceNumber + 1
    }
  }

  init() {
    self.ControlValue = 0
    self.EventSequenceNumber = 0
    self.AdminStoragePath = /storage/NodeVersionBeaconAdmin
    self.account.storage.save(<-create Admin(), to: self.AdminStoragePath)
  }
}