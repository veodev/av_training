#!/bin/bash
export PATH=/qt/5.11.0/gcc_64/bin:$PATH
lupdate ./avicon.pro -ts ./ui/widgets/internationalization/russian.ts
lupdate ./avicon.pro -ts ./ui/widgets/internationalization/german.ts
