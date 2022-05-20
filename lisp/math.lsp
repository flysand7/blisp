(do

  (def (gen-f int-f flt-f p1 p2)
    (cond (int? p1) (cond (int? p2) (int-f p1 p2)
                          (flt? p2) (flt-f (flt-from-int p1) p2)
                          1         'type_error)
          (flt? p1) (cond (int? p2) (flt-f p1 (flt-from-int p2))
                          (flt? p2) (flt-f p1 p2)
                          1         'type_error)
          1         'type_error))

  (def (foreach-accum init accum-func list)
    (cond (nil? list) init
          1           (accum-func (car list) (foreach-accum init accum-func (cdr list)))))

  (def (gen-add p1 p2)
    (gen-f int-add flt-add p1 p2))

  (def (gen-mul p1 p2)
    (gen-f int-mul flt-mul p1 p2))
  
  (def (gen-sub p1 p2)
    (gen-f int-sub flt-sub p1 p2))

  (def (gen-div p1 p2)
    (gen-f int-div flt-div p1 p2))

  (def (sum list)
    (foreach-accum 0 gen-add list))

  (def (mul list)
    (foreach-accum 1 gen-mul list))

  (def (+ . list)
    (sum list))

  (def (* . list)
    (mul list))

  (def - gen-sub)
  (def / gen-div)

  (print (+ 1 2 -0.2)))