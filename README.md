# 🎬 Cinemanet: Your Ultimate Cinema Ticket Booking Solution 🎟️

Welcome to **Cinemanet**, the premier cinema ticket booking system designed to make your movie-going experience seamless and enjoyable. Whether you're planning a solo trip to the movies or organizing a group outing, Cinemanet has got you covered!

---

## 📖 Table of Contents

- [About The Project](#about-the-project)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

---

## 🧐 About The Project

Cinemanet is a robust client-server application built in C, leveraging the power of MySQL for data management. It offers a comprehensive platform for users to browse films, select cinemas, choose shows, book seats, and generate tickets with ease. Designed with both users and administrators in mind, Cinemanet ensures a secure and efficient ticket booking process.

---

## 🌟 Features

- **User Authentication:**
  - Secure registration and login system.
  - Role-based access for regular users and administrators.

- **Film Browsing:**
  - Search films by title or category.
  - View detailed information about each film.

- **Cinema Selection:**
  - Browse cinemas by location.
  - View available shows for selected films.

- **Showtime Management:**
  - Select preferred showtimes.
  - Real-time seat availability updates.

- **Seat Booking:**
  - Interactive seat selection interface.
  - Prevents double bookings with transactional seat locking.

- **Ticket Generation:**
  - Generates detailed tickets with unique IDs.
  - Includes film, cinema, showtime, and seat details.

- **Admin Dashboard:**
  - Add, edit, or delete films and shows.
  - Manage seat maps and monitor bookings.

- **Secure Communication:**
  - Implements secure client-server communication protocols.

---


## 🛠️ Technologies Used

- **Programming Language:** C
- **Database:** MySQL
- **Libraries & Tools:**
  - MySQL Connector/C
  - POSIX Sockets for networking
  - pthreads for concurrency
- **Operating System:** Linux/Unix-based systems

---

## 🚀 Getting Started

Follow these instructions to set up and run Cinemanet on your local machine.

### 🔧 Prerequisites

Before you begin, ensure you have met the following requirements:

- **Operating System:** Linux or Unix-based OS.
- **Compiler:** Makefile
- **MySQL Server:** Installed and running.
- **Libraries:** MySQL Connector/C

### 📦 Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/yourusername/cinemanet.git
   cd cinemanet

2. **Set up the database:**

   ```bash
   mysql -u root -p < database/schema.sql
   
3. **Configure database credentials:**

    ```bash
   // database.c
  if (mysql_real_connect(conn_local, "localhost", "root", "yourpassword", "ticket_booking2", 0, NULL, 0) == NULL)

4. **Compile the program:**

   ```bash
   make clean
   make
   ./server port_number
   ./client IP_address port_number

5. **Thêm role admin:**

có 3 role, user là 1, seller là 0, admin là 2 (fix cứng trong database)
   ```bash
   ALTER TABLE users DROP CHECK users_chk_1;

  -- Or modify the constraint to include '2':
  ALTER TABLE users
  ADD CONSTRAINT users_chk_1 CHECK (role IN (0, 1, 2));

sau đó thêm 1 user có role = 2 vào để dùng làm admin.
admin có chức năng 8 - role assignment, để quản lý role các user khác.

