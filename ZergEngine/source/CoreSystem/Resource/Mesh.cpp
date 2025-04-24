#include <ZergEngine\CoreSystem\Resource\Mesh.h>
#include <ZergEngine\Common\EngineHelper.h>

using namespace ze;

Mesh::~Mesh()
{
	Helper::SafeReleaseCom(m_pVB);
	Helper::SafeReleaseCom(m_pIB);
}
