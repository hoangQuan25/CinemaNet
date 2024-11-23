

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

-- Sample data
-- Insert sample categories
INSERT INTO categories (category_name) VALUES
('Action'),
('Comedy'),
('Drama'),
('Horror'),
('Science Fiction'),
('Romance'),
('Animation');


-- Insert sample films
INSERT INTO films (film_name, category_id, description, length) VALUES
('The Great Adventure', 1, 'An action-packed journey across the globe.', 120),
('Laugh Out Loud', 2, 'A comedy that will have you rolling in the aisles.', 90),
('Tears of the Sun', 3, 'A dramatic tale of love and loss.', 110),
('Night Terrors', 4, 'A horror film that will keep you up at night.', 100),
('Space Odyssey', 5, 'A science fiction epic beyond imagination.', 130),
('Love in Paris', 6, 'A romantic story set in the city of love.', 105),
('Animated Dreams', 7, 'An animation that brings dreams to life.', 95);


-- Insert sample cinemas
INSERT INTO cinemas (cinema_name, seat_quantity) VALUES
('Cinema One', 25),
('Cinema Two', 30),
('Cinema Three', 20);


-- Insert sample shows
-- For current date, we'll use CURDATE() function, or specify dates.

-- Show for 'The Great Adventure' in 'Cinema One'
INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) VALUES
(1, 1, CURDATE(), '14:00:00', '16:00:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 1)));

-- Show for 'Laugh Out Loud' in 'Cinema Two'
INSERT INTO shows (film_id, cinema_id, date, start_date, end_time, seat_map) VALUES
(2, 2, CURDATE(), '15:00:00', '16:30:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 2)));

-- Show for 'Tears of the Sun' in 'Cinema Three'
INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) VALUES
(3, 3, CURDATE(), '16:00:00', '17:50:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 3)));

-- Add more shows as needed
-- Additional shows for 'Space Odyssey' in 'Cinema One' and 'Cinema Two'
INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) VALUES
(5, 1, CURDATE(), '18:00:00', '20:10:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 1))),
(5, 2, CURDATE(), '19:00:00', '21:10:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 2)));

-- Show for 'Love in Paris' in 'Cinema Three'
INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) VALUES
(6, 3, CURDATE(), '17:00:00', '18:45:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 3)));

-- Show for 'Animated Dreams' in all cinemas
INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) VALUES
(7, 1, CURDATE(), '12:00:00', '13:35:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 1))),
(7, 2, CURDATE(), '12:30:00', '14:05:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 2))),
(7, 3, CURDATE(), '13:00:00', '14:35:00', REPEAT('0', (SELECT seat_quantity FROM cinemas WHERE id = 3)));
