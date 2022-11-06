
; Problem: given 100 ruble bill and the coins with denumenations of 1 ruble,
; 2 rubles, 5 rubles and 10 rubles as well as 50 ruble "coin" count the number
; of ways the 100 ruble bill can be changed into coins.

; We'll assume that all kinds of coin are ordered in some way, so that we can
; speak of "the first kind of coin", "the second kind of coin" etc.

; We observe that the number of ways to make a change for the amount A can be
; calculated as a sum of the following:
; - The number of ways to make a change for the amount A using none of the
;   coins of the first kind (all kinds of coins but the coins of the first kind)
; - The number of ways to make a change for the amount A using at least one
;   coin of the first kind (that is, the number of ways to make a change for
;   the amount (A - d) with all kinds of coins, where d is the denomination of
;   the first kind of coin).

; And we'll define the following degenerate cases:
; - If A is zero we can change it using only one way.
; - If we don't use any coins we have 0 ways to make a change
; - If A is less than zero, we have 0 ways to make a change

(def (n-changes amount coins)
  (cond ((= amount 0) 1)
        ((= (count coins) 0) 0)
        ((< amount 0) 0)
        (else (+ (n-changes amount (rest coins))
                 (n-changes (- amount (first coins)) coins)))))

(print (n-changes 100 '(1 2 5 10 50)))
; This takes fucking 3 minutes to compute the answer: 2498