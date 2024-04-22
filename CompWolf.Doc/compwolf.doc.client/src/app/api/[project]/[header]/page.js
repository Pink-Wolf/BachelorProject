import { getHeader, getOverview } from "@/lib/api/Entity";
import HeaderViewer from "@/lib/api/HeaderViewer";

export default async function HeaderApi({ params }) {
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)

    const overview = await getOverview()
    const data = {
        ...(await getHeader(project, header)),
        entities: overview
            .projects.find((x) => { return x.name === project })
            .headers.find((x) => { return x.name === header })
            .entities
    }

    return <HeaderViewer data={data} />
}
