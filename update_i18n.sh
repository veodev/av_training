#!/bin/bash

lupdate avicon.pro -ts ./ui/widgets/internationalization/russian.ts
lrelease ./ui/widgets/internationalization/russian.ts -qm ./ui/widgets/internationalization/russian.qm
