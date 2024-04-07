import EntityViewer from "@/lib/api/EntityViewer";
import { getEntity } from "@/lib/api/Entity";

export default async function EntityPage({ params }) {
    const data = await getEntity(params.project, params.header, params.entity)

    return <EntityViewer data={data} />
}
