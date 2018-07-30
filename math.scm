(define not
  (lambda (x)
    (if x #f #t)))
(define round
  (lambda (x)
    (if (and (> (floor (+ x 0.5)) (floor x)))
        (ceiling x)
        (floor x))))
(define ceiling-helper
  (lambda (x)
    (if (negative? (- x 1))
        (if (= x 0) 0 1)
        (+ 1 (ceiling-helper (- x 1))))))
(define floor-helper
  (lambda (x)
    (if (negative? (- x 1)) 0
        (+ 1 (floor-helper (- x 1))))))
(define floor
  (lambda (x)
    (if (negative? x)
        (- 0 (ceiling-helper (abs x)))
        (floor-helper x))))
(define ceiling
  (lambda (x)
    (if (negative? x)
        (- 0 (floor-helper (abs x)))
        (ceiling-helper x))))
(define truncate
  (lambda (x)
    (if (negative? x)
        (ceiling x)
        (floor x))))
(define gcd
  (lambda (x y)
    (cond ((or (< x 0) (< y 0))
           (gcd (abs x) (abs y)))
          ((< x y)
           (gcd y x))
          ((= y 0) x)
          (else (gcd y (modulo x y))))))
(define lcm
  (lambda (x y)
    (/ (* (abs x) (abs y)) (gcd (abs x) (abs y)))))
(define abs
  (lambda (x)
    (if (negative? x)
        (- 0 x)
        x)))
(define min
  (lambda (x y)
    (if (<= x y)
        x
        y)))
(define max
  (lambda (x y)
    (if (>= x y)
        x
        y)))
(define >=
  (lambda (x y)
    (<= y x)))
(define =
  (lambda (x y)
    (and (<= (- x y) (- y x))
         (<= (- y x) (- x y)))))
(define zero?
  (lambda (x)
    (= x 0)))
(define modulo
  (lambda (x y)
    (if (< x y)
        x
        (modulo (- x y) y))))
(define <
  (lambda (x y)
    (and (<= x y) (not (= x y)))))
(define >
  (lambda (x y)
    (and (>= x y) (not (= x y)))))
(define positive?
  (lambda (x)
    (> x 0)))
(define negative?
  (lambda (x)
    (< x 0)))
(define even?
  (lambda (x)
    (= (modulo x 2) 0)))
(define odd?
  (lambda (x)
    (= (modulo x 2) 1)))
