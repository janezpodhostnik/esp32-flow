import MicrocontrollerTest from 0x4af24a49688eaa92

transaction(ledState: Bool) {
  let adminRef: &MicrocontrollerTest.Admin

  prepare(acct: auth(BorrowValue) &Account) {
    // Borrow a reference to the NodeVersionAdmin implementing resource
    self.adminRef = acct.storage.borrow<&MicrocontrollerTest.Admin>
      (from: MicrocontrollerTest.AdminStoragePath)
      ?? panic("Couldn't borrow Admin Resource")
  }

  execute {
    self.adminRef.setLed(ledState)
  }
}
