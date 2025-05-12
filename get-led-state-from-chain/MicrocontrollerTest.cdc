access(all) contract MicrocontrollerTest {

  access(all) let AdminStoragePath: StoragePath

  access(all) var LedOn: Bool

  access(all) resource Admin {
    access(all) fun setLed(_ on: Bool) {
      MicrocontrollerTest.LedOn = on
    }
  }

  init() {
    self.LedOn = false
    self.AdminStoragePath = /storage/NodeVersionBeaconAdmin
    self.account.storage.save(<-create Admin(), to: self.AdminStoragePath)
  }
}
