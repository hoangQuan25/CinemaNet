

-- Users Table
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(255) NOT NULL,
    username VARCHAR(100) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    role TINYINT(1) NOT NULL CHECK (role IN (0, 1)) -- 0: Admin, 1: User
);

-- Categories Table
CREATE TABLE categories (
    id INT AUTO_INCREMENT PRIMARY KEY,
    category_name VARCHAR(100) NOT NULL UNIQUE
);

-- Films Table
CREATE TABLE films (
    id INT AUTO_INCREMENT PRIMARY KEY,
    film_name VARCHAR(255) NOT NULL,
    category_id INT NOT NULL,
    description TEXT,
    length INT NOT NULL, -- Length in minutes
    FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE
);

-- Cinemas Table
CREATE TABLE cinemas (
    id INT AUTO_INCREMENT PRIMARY KEY,
    cinema_name VARCHAR(255) NOT NULL UNIQUE,
    seat_quantity INT NOT NULL
);

-- Shows Table
CREATE TABLE shows (
    id INT AUTO_INCREMENT PRIMARY KEY,
    film_id INT NOT NULL,
    cinema_id INT NOT NULL,
    date DATE NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    seat_map TEXT NOT NULL, -- A string representation of the seat map (e.g., '0 1 1 0')
    FOREIGN KEY (film_id) REFERENCES films(id) ON DELETE CASCADE,
    FOREIGN KEY (cinema_id) REFERENCES cinemas(id) ON DELETE CASCADE
);

-- Tickets Table
CREATE TABLE tickets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    film_id INT NOT NULL,
    cinema_id INT NOT NULL,
    show_id INT NOT NULL,
    seat_number INT NOT NULL, -- Individual seat number
    seat_list TEXT NOT NULL, -- List of seats booked (e.g., '1,2,5')
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (film_id) REFERENCES films(id) ON DELETE CASCADE,
    FOREIGN KEY (cinema_id) REFERENCES cinemas(id) ON DELETE CASCADE,
    FOREIGN KEY (show_id) REFERENCES shows(id) ON DELETE CASCADE
);
