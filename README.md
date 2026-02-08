# 📦 Smart Inventory System: ESP32 + Telegram + Google Sheets

An IoT-based inventory management system designed for domestic or small-scale environments. It allows users to track stock across multiple locations (Fridge, Pantry, Freezer, etc.) using **Telegram** as the user interface and **Google Sheets** as a serverless database.

## 🚀 Key Features

* **Multi-Command Processing**: Capable of parsing and executing multiple orders sent in a single Telegram message (separated by newlines).
* **Global Query (All-in-One)**: By sending a simple `c` command, the system iterates through all spreadsheet tabs and returns a consolidated inventory status.
* **Serverless Architecture**: Utilizes Google Apps Script (GAS) to handle logic and data persistence, eliminating the need for dedicated servers.
* **Smart Cleanup**: Automatically removes an item's row from the database when its quantity reaches zero during a `REMOVE` action.
* **Secure Communication**: Implements SSL/TLS encryption for all interactions between the ESP32, Telegram Bot API, and Google Services.
* **Robust Network Handling**: Custom handling of HTTP 302 redirects, a specific requirement for interacting with Google Web Apps.

## 🛠️ Tech Stack

* **Hardware**: ESP32 (NodeMCU / DevKit V1).
* **Language**: C++ (Arduino Framework).
* **Backend**: JavaScript (Google Apps Script).
* **Database**: Google Sheets.
* **UI/UX**: Telegram Bot API.

## 📋 Command Syntax

The custom-built parser interprets the following formats (Note: The parser is specifically designed to process commands in Spanish):

| Action | Syntax | Example |
| :--- | :--- | :--- |
| **Add** | `a [qty] [item] [location]` | `a 2 milk heladera` |
| **Remove** | `q [qty] [item] [location]` | `q 1 butter freezer` |
| **Query Sheet** | `c [location]` | `c despensa` |
| **Global Query** | `c` | `c` (Returns all items from all sheets) |

## ⚙️ Setup Instructions

### 1. Google Sheets & Script Configuration
1.  Create a new Google Sheet and name tabs according to your locations (e.g., `fridge`, `pantry` — use lowercase).
2.  Navigate to `Extensions > Apps Script` and paste the provided `doPost.js` code.
3.  Deploy as a **Web App**:
    * **Execute as**: Me.
    * **Who has access**: Anyone.
4.  Copy the generated **Web App URL**.

### 2. Telegram Bot Setup
1.  Message [@BotFather](https://t.me/botfather) on Telegram to create a bot and obtain your `BOT_TOKEN`.
2.  Retrieve your `CHAT_ID` to ensure only authorized users can modify the inventory.

### 3. ESP32 Firmware
Create a `include/credentials.h` file (excluded from Git) with your private information (see `include/credentials_template.h`)

## Project Structure
- `main.cpp`: Core logic for bot polling, message fragmentation, and task execution.
- `Parser.cpp`: Custom string analyzer that converts raw text into Command structures.
- `NetworkHandler.cpp`: Manages HTTPS requests and handles the mandatory 302 redirect logic for Google.
- `script_sheets`.js: The Google Apps Script logic (included in this repo for reference).

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## Contributing & Feedback

This is an open-source project created as part of my journey in Electronics Engineering. **Feel free to fork this repository, open issues, or submit pull requests** if you have any suggestions, improvements, or bug fixes. Your feedback is highly appreciated!