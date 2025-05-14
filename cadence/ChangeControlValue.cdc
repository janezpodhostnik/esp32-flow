import MicrocontrollerTest from "MicrocontrollerTest"

transaction(newValue: Int64) {
  let adminRef: &MicrocontrollerTest.Admin

  prepare(acct: auth(BorrowValue) &Account) {
    // Borrow a reference to the NodeVersionAdmin implementing resource
    self.adminRef = acct.storage.borrow<&MicrocontrollerTest.Admin>
      (from: MicrocontrollerTest.AdminStoragePath)
      ?? panic("Couldn't borrow Admin Resource")
  }

  execute {
    self.adminRef.setControlValue(newValue)
  }
}