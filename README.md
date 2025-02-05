# restaurant-arduino
Arduino-based project featuring an I2C OLED display and an HC-12 wireless module to send time and status updates to the host stand in a restaurant environment

## Wings Branch - Inventory Tracking Mode

This branch repurposes the Arduino-based transmitter unit to track inventory for high-demand food items, such as wings during events like Super Bowl Sunday. Instead of transmitting data via the HC-12 module, the device functions as a local counter with an OLED display.

### **Functionality Overview**
- **Inventory Tracking:** Starts with an initial quantity (default: `1000`).
- **Decrement Mode Toggle:** The **call switch (ON/OFF)** toggles between decrementing by `6` (standard orders) and `25` (catering orders).
- **Adjustment Controls:** 
  - The **up momentary switch** **increases** inventory by the selected decrement value.
  - The **down momentary switch** **decreases** inventory by the selected decrement value.
- **OLED Display:** 
  - The current inventory count is displayed in **large text**.
  - The active decrement mode (`-6 Mode` or `-25 Mode`) is shown at the **top in small text**.
- **Standalone Mode:** The HC-12 module is not used in this branch; the device only updates the display.

This temporary modification ensures that restaurant staff can quickly track remaining wing inventory and prevent overselling during peak demand.

---
