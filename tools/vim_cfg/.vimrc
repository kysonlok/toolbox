" This must be first, because it changes other options as a side effect.
set nocompatible
set nocp
set number
set relativenumber

" allow backspacing over everything in insert mode
set backspace=indent,eol,start

set history=50    " keep 50 lines of command line history
set ruler         " show the cursor position all the time
set showcmd       " display incomplete commands
set incsearch     " do incremental searching

set tabstop=4     " 设置制表符(tab键)的宽度
set softtabstop=4 " 设置软制表符的宽度
set shiftwidth=4  " (自动) 缩进使用的4个空格
set expandtab
set cindent       " 使用 C/C++ 语言的自动缩进方式

set completeopt=longest,menu " 去掉自动补全的预览窗口

" 改变自动补全窗口的配色
" GUI
highlight Pmenu    guibg=darkgrey  guifg=black 
highlight PmenuSel guibg=lightgrey guifg=black
" 终端
highlight Pmenu    ctermfg=darkgrey  ctermbg=black
highlight PmenuSel ctermfg=lightgrey ctermbg=green

" 显示中文
set fencs=utf-8,GB18030,ucs-bom,default,latin1
set nobackup    " 不备份
set ignorecase  " 忽略大小写的查找

set tw=80       " set textwidth

if has("autocmd")

    " If the filetype is Makefile then we need to use tabs
    " So do not expand tabs into space.
    autocmd FileType make   set noexpandtab

endif

" Uncomment the following to have Vim jump to the last position when
" reopening a file
if has("autocmd")
  au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif

au BufNewFile,BufRead *.py
    \ set tabstop=4 |
    \ set softtabstop=4 |
    \ set shiftwidth=4 |
    \ set textwidth=79 |
    \ set noexpandtab |
    \ set autoindent |
    \ set fileformat=unix
