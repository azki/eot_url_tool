eot_url_tool
============

eot 포맷의 웹폰트의 라이센스 URL을 확인하거나, 변경할 수 있는 툴입니다.
해당 파일의 스펙은 http://www.w3.org/Submission/EOT/ 에서 참고하였습니다.


1. 등록된 URL을 읽기 위해서는.
```cmd
> eot_url_tool in.eot
```

2. URL을 새로 세팅하기 위해서는.
```cmd
> eot_url_tool in.eot out.eot http://daum.net http://uie.daum.net
```

ex)
```cmd
C:\>eot_url_tool
usage: eot_url_tool input_file [output_file [URL [URL...]]]

ex1. read urls.
        eot_url_tool in.eot
ex2. set urls.
        eot_url_tool in.eot out.eot http://daum.net http://uie.daum.net


C:\>eot_url_tool YDgirl.eot
http://daum.net http://www.yoonfont.co.kr
C:\>eot_url_tool YDgirl.eot YDgirl2.eot http://azki.org
complete.
C:\>eot_url_tool YDgirl2.eot
http://azki.org
C:\>
```