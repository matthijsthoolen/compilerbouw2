set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'
Plugin 'scrooloose/nerdtree'
Plugin 'octol/vim-cpp-enhanced-highlight'
Plugin 'hzchirs/vim-material'

call vundle#end()

set visualbell

set mouse=n
set number
set tabstop=4
set softtabstop=4
set expandtab
set cursorline
set wildmenu
set showmatch

set incsearch
set hlsearch
set ignorecase
set smartcase
set showmatch

set foldenable

colorscheme vim-material

syntax enable

" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line

map <F2> :NERDTreeToggle<CR>
map <F3> :NERDTreeFind<CR>
