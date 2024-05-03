import { getOverview } from "@/lib/api/Entity";
import ProjectListViewer from "@/lib/api/ProjectListViewer";

export default async function ApiPage({ }) {
    const overview = await getOverview()

    return <ProjectListViewer data={overview} />
}
