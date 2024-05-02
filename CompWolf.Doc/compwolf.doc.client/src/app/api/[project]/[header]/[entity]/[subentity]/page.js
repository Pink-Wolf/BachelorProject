import { getOverview, getEntity } from "@/lib/api/Entity";
import EntityViewer from "@/lib/api/EntityViewer";
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent";

export default async function SubEntityPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)
    let subentity = decodeURIComponent(params.subentity)

    const owner = await getEntity(project, header, entity)
    const target = subentity

    var data = undefined
    if (target == entity) {
        data = { ...owner.constructor, type: "function" }
    } else owner.memberGroups?.find((i) => {
        i.items.find((j) => {
            if (j.name === target) data = j

            return (data !== undefined)
        })
        return (data !== undefined)
    })

    data = {
        ...data,
        project: owner.project,
        header: owner.header,
        owner: owner.name,
        namespace: owner.namespace,
        name: target,
    }

    return <EntityViewer data={data} />
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header =>
            header.entities.map(entity =>
                entity.members.map(member => {
                    return {
                        project: betterEncodeURIComponent(project.name),
                        header: betterEncodeURIComponent(header.name),
                        entity: betterEncodeURIComponent(entity.name),
                        subentity: betterEncodeURIComponent(member.name),
                    }
                })
            )
        )
    ).flat(Infinity)
}
