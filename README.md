Fork of **bamtools** from <https://github.com/pezmaster31/bamtools/>.  I love
the bamtools API and will put mods here that could potentially be part of the
general bamtools offering.  More extensive mods (such as my desire to create a
version that can be dropped into an existing Makefile-driven project) will go
in another fork.

#### Reduced header-associated memory usage

I added the methods
```c++
const SamHeader& BamReader::GetConstSamHeader() const { }
```
to the public interface for BamReader,
```c++
const SamHeader& BamReaderPrivate::GetConstSamHeader() const { }
```
to the public interface for Internal::BamReaderPrivate and
```c++
const SamHeader& BamHeader::ToConstSamHeader() const { }
```
to the Internal::BamHeader class, this is the method that ultimately provides
the reference requested by BamReader::GetConstSamHeader().

