Qt requirements

Важно: для билда Qt необходимо установить студию 15.0, на ней билдится точно, на других версиях возникают ошибки компиляции (что то вроде "внутренняя ошибка компилятора").
Не менее важно: необходимо установить настройку гита git config --global core.autocrlf false. Если этого не сделать файлы сертификатов из сорсов хромиума скачаются с неправильными переносами строк и билд зафейлится.
Интересная особенность: почему то нормально конфигурируется кьют только если опенссл лежит в папке c:\openssl. Поэтому и x64 и win32 сначала устанавливаются туда и только потом копируются в папку депсов.

Openssl requirements

- Perl. We recommend ActiveState Perl, available from
https://www.activestate.com/ActivePerl. Another viable alternative
appears to be Strawberry Perl, http://strawberryperl.com.
You also need the perl module Text::Template, available on CPAN.
Please read NOTES.PERL for more information.

- Microsoft Visual C compiler. Since we can't test them all, there is
unavoidable uncertainty about which versions are supported. Latest
version along with couple of previous are certainly supported. On
the other hand oldest one is known not to work. Everything between
falls into best-effort category.

- Netwide Assembler, a.k.a. NASM, available from http://www.nasm.us,
is required if you intend to utilize assembler modules. Note that NASM
is the only supported assembler. The Microsoft provided assembler is NOT
supported.

ICU

Requirements
MSVC toolchain
Cygwin
Note: When installing Cygwin and selecting packages, make sure you search for and select 'make', 'dos2unix' and 'binutils'.
Download icu4c-58_2-src from  http://site.icu-project.org/download


Boost

Почему то не получилось закомпилить из сорсов скачанных с гитхаба, поэтому все собиралось из архива который можно найти здесь: https://yadi.sk/d/I4qfKFy03TFN4g

Остальное можно понять по батникам.