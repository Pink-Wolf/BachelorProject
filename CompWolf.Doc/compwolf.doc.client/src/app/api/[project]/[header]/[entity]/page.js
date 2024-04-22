import EntityViewer from "@/lib/api/EntityViewer";
import { getEntity } from "@/lib/api/Entity";

export default async function EntityPage({ params }) {
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)

    const data = await getEntity(project, header, entity)

    return <EntityViewer data={data} />
}
