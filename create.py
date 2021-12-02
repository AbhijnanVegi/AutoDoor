from onem2m import *

uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"
ae = "Spoodergang"
cnt = "MotionDetected"

create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae,cnt)
uri_cnt = uri_ae+"/" +cnt

