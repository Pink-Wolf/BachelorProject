import ClassViewer from "@/lib/api/ClassViewer";
import { getEntity } from "@/lib/api/Entity";

export default async function ApiPage({ params }) {
    const data = await getEntity(params.project, params.header, params.entity)

    return <ClassViewer data={data} />
}
