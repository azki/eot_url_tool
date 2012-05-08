eot_url_tool 1.1
=============

Microsoft WEFT로 만든 웹폰트의 URL을 변경시켜주는 프로그램입니다.
eot 포맷의 웹폰트의 라이선스 URL을 확인하거나, 변경할 수 있습니다.
해당 파일의 스펙은 http://www.w3.org/Submission/EOT/ 에서 참고하였습니다.

Version History
1.0 : 안녕.
1.1 : 읽은 파일로 쓰기 가능.


1. 등록된 URL을 읽기 위해서는.
```
> eot_url_tool in.eot
```

2. URL을 새로 세팅하기 위해서는.
```
> eot_url_tool in.eot out.eot http://daum.net http://uie.daum.net
```

ex)
```
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
