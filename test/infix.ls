(do
  (include env 'test/ops.ls)

  (define infix (lambda ('...)
    
    (define term-expr (lambda (expr)
      (cond (== (car expr) '-) (cons (- 1 (car(cdr(expr)))) (cdr (cdr expr)))
            (num? (car expr))  (cons (car expr) (cdr expr))
            (list? (car expr)) (sum-expr (car expr)))))
    
    (define mul-expr (lambda (expr)
      (define op1-str (term-expr expr))
      (define op1 (car op1-str))
      (define expr (cdr op1-str))
      (if (and (nil? expr) (== (car expr) *))
        op1
        (do
          (define fn  (eval env (car expr)))
          (define expr (cdr expr))
          (define op2-str (term-expr expr))
          (define op2 (car op2-str))
          (define expr (cdr op2-str))
          (fn op1 op2)))))

    (define sum-expr (lambda (expr)
      (define op1-str (mul-expr expr))
      (define op1 (car op1-str))
      (define expr (cdr op1-str))
      (if (and (nil? expr) (== (car expr) +))
        op1
        (do
          (define fn  (eval env (car expr)))
          (define expr (cdr expr))
          (define op2-str (mul-expr expr))
          (define op2 (car op2-str))
          (define expr (cdr op2-str))
          (fn op1 op2)))))

    (define expr ...)
    (sum-expr expr))))
    