#!/bin/bash

find . -type f \( -name "*.ll" -o -name "*.istats" \) -exec rm {} \;
echo "已删除所有.ll和.istats文件."

