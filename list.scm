(define foldl
  (lambda (f init lst)
    (if (null? lst)
        init
        (foldl f
               (f (car lst) init)
               (cdr lst)))))
(define foldr
  (lambda (f init lst)
    (if (null? lst)
        init
        (f (car lst)
           (foldr f
                  init
                  (cdr lst))))))
(define filter
  (lambda (f l)
    (if (null? l) '()
        (if (f (car l))
            (cons (car l) (filter f (cdr l)))
            (filter f (cdr l))))))
(define map
  (lambda (f l)
    (if (null? l) '()
        (cons (f (car l)) (map f (cdr l))))))
(define caar (lambda (x) (car (car x))))
(define cadr (lambda (x) (car (cdr x))))
(define cddr (lambda (x) (cdr (cdr x))))
(define cdddr (lambda (x) (cdr (cdr (cdr x)))))
(define cddddr (lambda (x) (cdr (cdr (cdr (cdr x))))))
(define list (lambda x x))

(define append
  (lambda (lst1 lst2)
    (if (null? lst1)
        lst2
(cons (car lst1) (append (cdr lst1) lst2)))))
(define length
  (lambda (lst)
    (if (null? lst)
        0
        (+ 1 (length (cdr lst))))))
(define reverse
  (lambda (lst)
    (if (null? lst)
        lst
        (append (reverse (cdr lst)) (cons (car lst) (quote ()))))))
(define memq
  (lambda (x ls)
    (cond
      ((null? ls) #f)
      ((eq? (car ls) x) ls)
      (else (memq x (cdr ls))))))
(define list-ref
  (lambda (x n) (if (= n 0)
                    (car x)
                    (list-ref (cdr x) (- n 1)))))
(define list-tail
  (lambda (ls n)
    (if (= n 0)
        ls
        (list-tail (cdr ls) (- n 1)))))
(define member
  (lambda (x ls)
    (cond
      ((null? ls) #f)
      ((eq? (car ls) x) ls)
      (else (member x (cdr ls))))))
(define assq
  (lambda (x ls)
    (cond
      ((null? ls) #f)
      ((eq? (caar ls) x) (car ls))
      (else (assq x (cdr ls))))))

(define equal?
  (lambda (x y)
  (cond ((and (pair? x) (pair? y)) (and (equal? (car x) (car y))
                                          (equal? (cdr x) (cdr y))))
        (else (eq? x y)))))
