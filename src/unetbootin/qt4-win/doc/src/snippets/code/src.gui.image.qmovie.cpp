//! [0]
        QLabel label;
        QMovie *movie = new QMovie("animations/fire.gif");

        label.setMovie(movie);
        movie->start();
//! [0]


//! [1]
        QMovie movie("racecar.gif");
        movie.setSpeed(200); // 2x speed
//! [1]


