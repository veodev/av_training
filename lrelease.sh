#!/bin/bash
export PATH=/qt/5.11.0/gcc_64/bin:$PATH
lrelease ./ui/widgets/internationalization/russian.ts -qm ./ui/widgets/internationalization/russian.qm
lrelease ./ui/widgets/internationalization/german.ts -qm ./ui/widgets/internationalization/german.qm
