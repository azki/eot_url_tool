eot_url_tool
============

webfont (eot) file&#39;s URL view and change(get RootString, set RootString).


eot 포맷의 웹폰트의 라이센스 URL을 확인하거나, 변경할 수 있는 툴입니다.
해당 파일의 스펙은 http://www.w3.org/Submission/EOT/ 에서 참고하였습니다.



등록된 URL을 읽기 위해서는.
```cmd
> eot_url_tool in.eot
```

URL을 새로 세팅하기 위해서는.
```cmd
> eot_url_tool in.eot out.eot http://daum.net http://uie.daum.net
```