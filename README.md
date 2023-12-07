# deterministic-builds-msvc

Utility for creating deterministic builds under Microsoft Visual C++

## Target features

- [ ] Modify signatures for outputs and PDB
- [ ] Modify date times in outputs and PDB
- [ ] Modify `__DATE__`, `__TIME__` and `__TIMESTAMP__` to custom date
- [ ] Modify `__FILE__` macro to remove full path

## References

### MSVC Compiler/Linker flags

- `/experimental:deterministic` - expermental flag - deterministic builds (do not modify any macro)
- `/Brepro` - undocumented linker & compiler flag sets `__DATA__="1"`, `__TIME__="1"`, `__TIMESTAMP="1"`
- `/d1trimfile:<path>` - trim source path from `__FILE__` macro & pdb file location

### Related projects

- Microsoft PDB [https://github.com/microsoft/microsoft-pdb](https://github.com/microsoft/microsoft-pdb)
- Dubicle [https://github.com/jasonwhite/ducible](https://github.com/jasonwhite/ducible) - fixes date, time and PDB signatures

### References

- Getting to Deterministic Builds on Windows - Nikhil's blog [https://nikhilism.com/post/2020/windows-deterministic-builds/](https://nikhilism.com/post/2020/windows-deterministic-builds/)
- Why are the module timestamps in Windows 10 so nonsensical? - The Old New Thing [https://devblogs.microsoft.com/oldnewthing/20180103-00/?p=97705](https://devblogs.microsoft.com/oldnewthing/20180103-00/?p=97705)
