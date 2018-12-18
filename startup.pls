(begin 

    (define point (lambda (xCoordinate yCoordinate) (set-property "object-name" "point" (list xCoordinate yCoordinate))))

    (define make-point (lambda (x y) ((set-property "size" (0) (point x y))))

    (define line (lambda (pointA pointB) (set-property "object-name" "line" (list pointA pointB))))

    (define make-line (lambda (a b) ((set-property "thickness" (1) (line a b))))

    (define text (lambda (textString) (set-property "object-name" "text"(set-property "scale" (1) (set-property "text-rotation" (0) (textString))))))

    (define make-text (lambda (str) ((set-property "position" (make-point 0 0) (text str))))

) 
